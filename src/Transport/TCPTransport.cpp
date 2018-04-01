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

#include <f1x/aasdk/Transport/TCPTransport.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

TCPTransport::TCPTransport(boost::asio::io_service& ioService, tcp::ITCPEndpoint::Pointer tcpEndpoint)
    : Transport(ioService)
    , tcpEndpoint_(std::move(tcpEndpoint))
{

}

void TCPTransport::enqueueReceive(common::DataBuffer buffer)
{
    auto receivePromise = tcp::ITCPEndpoint::Promise::defer(receiveStrand_);
    receivePromise->then([this, self = this->shared_from_this()](auto bytesTransferred) {
            this->receiveHandler(bytesTransferred);
        },
        [this, self = this->shared_from_this()](auto e) {
            this->rejectReceivePromises(e);
        });

    tcpEndpoint_->receive(buffer, std::move(receivePromise));
}

void TCPTransport::enqueueSend(SendQueue::iterator queueElement)
{
    auto sendPromise = tcp::ITCPEndpoint::Promise::defer(sendStrand_);

    sendPromise->then([this, self = this->shared_from_this(), queueElement](auto) {
        this->sendHandler(queueElement, error::Error());
    },
    [this, self = this->shared_from_this(), queueElement](auto e) {
        this->sendHandler(queueElement, e);
    });

    tcpEndpoint_->send(common::DataConstBuffer(queueElement->first), std::move(sendPromise));
}

void TCPTransport::stop()
{
    tcpEndpoint_->stop();
}

void TCPTransport::sendHandler(SendQueue::iterator queueElement, const error::Error& e)
{
    if(!e)
    {
        queueElement->second->resolve();
    }
    else
    {
        queueElement->second->reject(e);
    }

    sendQueue_.erase(queueElement);

    if(!sendQueue_.empty())
    {
        this->enqueueSend(sendQueue_.begin());
    }
}

}
}
}
