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
    : aoapDevice_(std::move(aoapDevice))
    , receiveStrand_(ioService)
    , sendStrand_(ioService)
{}

void USBTransport::receive(size_t size, ReceivePromise::Pointer promise)
{
    receiveStrand_.dispatch([this, self = this->shared_from_this(), size, promise = std::move(promise)]() mutable {
        receiveQueue_.emplace_back(std::make_pair(size, std::move(promise)));

        if(receiveQueue_.size() == 1)
        {
            try
            {
                this->distributeReceivedData();
            }
            catch(const error::Error& e)
            {
                this->rejectReceivePromises(e);
            }
        }
    });
}

void USBTransport::receiveHandler(size_t bytesTransferred)
{
    try
    {
        usbReceivedDataSink_.commit(bytesTransferred);
        this->distributeReceivedData();
    }
    catch(const error::Error& e)
    {
        this->rejectReceivePromises(e);
    }
}

void USBTransport::distributeReceivedData()
{
    for(auto queueElement = receiveQueue_.begin(); queueElement != receiveQueue_.end();)
    {
        if(usbReceivedDataSink_.getAvailableSize() < queueElement->first)
        {
            auto buffer = usbReceivedDataSink_.fill();

            auto usbEndpointPromise = usb::IUSBEndpoint::Promise::defer(receiveStrand_);
            usbEndpointPromise->then(std::bind(&USBTransport::receiveHandler, this->shared_from_this(), std::placeholders::_1),
                                    std::bind(&USBTransport::rejectReceivePromises, this->shared_from_this(), std::placeholders::_1));
            aoapDevice_->getInEndpoint().bulkTransfer(buffer, cReceiveTimeoutMs, std::move(usbEndpointPromise));
            break;
        }
        else
        {
            auto data(usbReceivedDataSink_.consume(queueElement->first));
            queueElement->second->resolve(std::move(data));
            queueElement = receiveQueue_.erase(queueElement);
        }
    }
}

void USBTransport::rejectReceivePromises(const error::Error& e)
{
    for(auto& queueElement : receiveQueue_)
    {
        queueElement.second->reject(e);
    }

    receiveQueue_.clear();
}

void USBTransport::send(common::Data data, SendPromise::Pointer promise)
{    
    sendStrand_.dispatch([this, self = this->shared_from_this(), data = std::move(data), promise = std::move(promise)]() mutable {
        sendQueue_.emplace_back(std::make_pair(std::move(data), std::move(promise)));

        if(sendQueue_.size() == 1)
        {
            this->doSend(sendQueue_.begin(), 0);
        }
    });
}

void USBTransport::doSend(OutTransferQueue::iterator queueElementIter, common::Data::size_type offset)
{
    auto usbEndpointPromise = usb::IUSBEndpoint::Promise::defer(sendStrand_);
    usbEndpointPromise->then([this, self = this->shared_from_this(), queueElementIter, offset](size_t bytesTransferred) mutable {
            this->sendHandler(queueElementIter, offset, bytesTransferred);
        },
        [this, self = this->shared_from_this(), queueElementIter](const error::Error& e) mutable {
            queueElementIter->second->reject(e);
            sendQueue_.erase(queueElementIter);

            if(!sendQueue_.empty())
            {
                this->doSend(sendQueue_.begin(), 0);
            }
        });
    aoapDevice_->getOutEndpoint().bulkTransfer(common::DataBuffer(queueElementIter->first, offset), cSendTimeoutMs, std::move(usbEndpointPromise));
}

void USBTransport::sendHandler(OutTransferQueue::iterator queueElementIter, common::Data::size_type offset, size_t bytesTransferred)
{
    if(offset + bytesTransferred < queueElementIter->first.size())
    {
        this->doSend(queueElementIter, offset + bytesTransferred);
    }
    else
    {
        queueElementIter->second->resolve();
        sendQueue_.erase(queueElementIter);

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
