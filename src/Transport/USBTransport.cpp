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

#include <f1x/aasdk/Transport/USBTransport.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

USBTransport::USBTransport(boost::asio::io_service& ioService, usb::IAOAPDevice::Pointer aoapDevice)
    : Transport(ioService)
    , aoapDevice_(std::move(aoapDevice))
{}

void USBTransport::enqueueReceive(common::DataBuffer buffer)
{
    auto usbEndpointPromise = usb::IUSBEndpoint::Promise::defer(receiveStrand_);
    usbEndpointPromise->then([this, self = this->shared_from_this()](auto bytesTransferred) {
            this->receiveHandler(bytesTransferred);
        },
        [this, self = this->shared_from_this()](auto e) {
            this->rejectReceivePromises(e);
        });

    aoapDevice_->getInEndpoint().bulkTransfer(buffer, cReceiveTimeoutMs, std::move(usbEndpointPromise));
}

void USBTransport::enqueueSend(SendQueue::iterator queueElement)
{
    this->doSend(queueElement, 0);
}

void USBTransport::doSend(SendQueue::iterator queueElement, common::Data::size_type offset)
{
    auto usbEndpointPromise = usb::IUSBEndpoint::Promise::defer(sendStrand_);
    usbEndpointPromise->then([this, self = this->shared_from_this(), queueElement, offset](size_t bytesTransferred) mutable {
            this->sendHandler(queueElement, offset, bytesTransferred);
        },
        [this, self = this->shared_from_this(), queueElement](const error::Error& e) mutable {
            queueElement->second->reject(e);
            sendQueue_.erase(queueElement);

            if(!sendQueue_.empty())
            {
                this->doSend(sendQueue_.begin(), 0);
            }
        });

    aoapDevice_->getOutEndpoint().bulkTransfer(common::DataBuffer(queueElement->first, offset), cSendTimeoutMs, std::move(usbEndpointPromise));
}

void USBTransport::sendHandler(SendQueue::iterator queueElement, common::Data::size_type offset, size_t bytesTransferred)
{
    if(offset + bytesTransferred < queueElement->first.size())
    {
        this->doSend(queueElement, offset + bytesTransferred);
    }
    else
    {
        queueElement->second->resolve();
        sendQueue_.erase(queueElement);

        if(!sendQueue_.empty())
        {
            this->doSend(sendQueue_.begin(), 0);
        }
    }
}

void USBTransport::stop()
{
    aoapDevice_->getInEndpoint().cancelTransfers();
    aoapDevice_->getOutEndpoint().cancelTransfers();
}

}
}
}
