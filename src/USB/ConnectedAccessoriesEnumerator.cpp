/*
*  This file is part of aasdk library project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  aasdk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  aasdk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with aasdk. If not, see <http://www.gnu.org/licenses/>.
*/

#include <f1x/aasdk/USB/ConnectedAccessoriesEnumerator.hpp>

namespace f1x
{
namespace aasdk
{
namespace usb
{

ConnectedAccessoriesEnumerator::ConnectedAccessoriesEnumerator(IUSBWrapper& usbWrapper, boost::asio::io_service& ioService, IAccessoryModeQueryChainFactory& queryChainFactory)
    : usbWrapper_(usbWrapper)
    , strand_(ioService)
    , queryChainFactory_(queryChainFactory)
{

}

void ConnectedAccessoriesEnumerator::enumerate(Promise::Pointer promise)
{
    strand_.dispatch([this, self = this->shared_from_this(), promise = std::move(promise)]() mutable {
        if(promise_ != nullptr)
        {
            promise->reject(error::Error(error::ErrorCode::OPERATION_IN_PROGRESS));
        }
        else
        {
            promise_ = std::move(promise);

            auto result = usbWrapper_.getDeviceList(deviceListHandle_);

            if(result < 0)
            {
                promise_->reject(error::Error(error::ErrorCode::USB_LIST_DEVICES));
            }
            else if(deviceListHandle_->empty())
            {
                promise_->resolve(false);
            }
            else
            {
                actualDeviceIter_ = deviceListHandle_->begin();
                this->queryNextDevice();
            }
        }
    });
}

void ConnectedAccessoriesEnumerator::cancel()
{
    strand_.dispatch([this, self = this->shared_from_this()]() mutable {
        if(queryChain_ != nullptr)
        {
            queryChain_->cancel();
        }
    });
}

void ConnectedAccessoriesEnumerator::queryNextDevice()
{
    auto deviceHandle = this->getNextDeviceHandle();

    if(deviceHandle != nullptr)
    {
        queryChain_ = queryChainFactory_.create();
        auto queryChainPromise = IAccessoryModeQueryChain::Promise::defer(strand_);

        queryChainPromise->then([this, self = this->shared_from_this()](DeviceHandle) mutable {
                promise_->resolve(true);
                this->reset();
            },
            [this, self = this->shared_from_this()](const error::Error& e) mutable {
                if(e != error::ErrorCode::OPERATION_ABORTED)
                {
                    this->queryNextDevice();
                }
                else
                {
                    promise_->reject(e);
                    this->reset();
                }
            });

        queryChain_->start(std::move(deviceHandle), std::move(queryChainPromise));
    }
    else if(actualDeviceIter_ == deviceListHandle_->end())
    {
        promise_->resolve(false);
        this->reset();
    }
}

DeviceHandle ConnectedAccessoriesEnumerator::getNextDeviceHandle()
{
    DeviceHandle handle;

    while(actualDeviceIter_ != deviceListHandle_->end())
    {
        auto openResult = usbWrapper_.open(*actualDeviceIter_, handle);
        ++actualDeviceIter_;

        if(openResult == 0)
        {
            break;
        }
    }

    return handle;
}

void ConnectedAccessoriesEnumerator::reset()
{
    queryChain_.reset();
    deviceListHandle_.reset();
    actualDeviceIter_ = DeviceList::iterator();
    promise_.reset();
}

}
}
}
