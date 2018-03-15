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
    : ioService_(ioService)
    , tcpEndpoint_(std::move(tcpEndpoint))
    , receiveStrand_(ioService_)
    , sendStrand_(ioService_)
{

}

void TCPTransport::receive(size_t size, ReceivePromise::Pointer promise)
{
    receiveStrand_.dispatch([this, self = this->shared_from_this(), size, promise = std::move(promise)]() mutable {
        receiveQueue_.emplace_back(std::make_pair(size, std::move(promise)));

        if(receiveQueue_.size() == 1)
        {
            this->distributeReceivedData();
        }
    });
}

void TCPTransport::send(common::Data data, SendPromise::Pointer promise)
{
    sendStrand_.dispatch([this, self = this->shared_from_this(), data = std::move(data), promise = std::move(promise)]() mutable {
        sendQueue_.emplace_back(std::make_pair(std::move(data), std::move(promise)));

        if(sendQueue_.size() == 1)
        {
            this->doSend();
        }
    });
}

void TCPTransport::stop()
{
    tcpEndpoint_->stop();
}

void TCPTransport::receiveHandler(size_t bytesTransferred)
{
    try
    {
        tcpReceivedDataSink_.commit(bytesTransferred);
        this->distributeReceivedData();
    }
    catch(const error::Error& e)
    {
        //this->rejectReceivePromises(e);
    }
}

void TCPTransport::receiveFailureHandler(const aasdk::error::Error& e)
{
}

void TCPTransport::doSend()
{
    auto queueElement = sendQueue_.begin();

    auto sendPromise = tcp::ITCPEndpoint::Promise::defer(sendStrand_);
    sendPromise->then(std::bind(&TCPTransport::sendHandler, this->shared_from_this(), std::placeholders::_1, queueElement),
                      std::bind(&TCPTransport::sendFailureHandler, this->shared_from_this(), std::placeholders::_1, queueElement));

    tcpEndpoint_->send(common::DataConstBuffer(queueElement->first), std::move(sendPromise));
}

void TCPTransport::sendHandler(size_t, SendQueue::iterator queueElement)
{
    queueElement->second->resolve();
    sendQueue_.erase(queueElement);

    if(!sendQueue_.empty())
    {
        this->doSend();
    }
}

void TCPTransport::sendFailureHandler(const aasdk::error::Error& e, SendQueue::iterator queueElement)
{
    queueElement->second->reject(e);
    sendQueue_.erase(queueElement);

    if(!sendQueue_.empty())
    {
        this->doSend();
    }
}

void TCPTransport::distributeReceivedData()
{
    for(auto queueElement = receiveQueue_.begin(); queueElement != receiveQueue_.end();)
    {
        if(tcpReceivedDataSink_.getAvailableSize() < queueElement->first)
        {
            auto buffer = tcpReceivedDataSink_.fill();

            auto receivePromise = tcp::ITCPEndpoint::Promise::defer(receiveStrand_);
            receivePromise->then(std::bind(&TCPTransport::receiveHandler, this->shared_from_this(), std::placeholders::_1),
                                 std::bind(&TCPTransport::receiveFailureHandler, this->shared_from_this(), std::placeholders::_1));
            tcpEndpoint_->receive(buffer, std::move(receivePromise));
            break;
        }
        else
        {
            auto data(tcpReceivedDataSink_.consume(queueElement->first));
            queueElement->second->resolve(std::move(data));
            queueElement = receiveQueue_.erase(queueElement);
        }
    }
}

}
}
}
