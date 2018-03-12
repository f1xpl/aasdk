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
            this->doReceive();
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

void TCPTransport::doReceive()
{
    auto queueElement = receiveQueue_.begin();
    receiveData_.resize(queueElement->first);

    auto receivePromise = tcp::ITCPEndpoint::Promise::defer(receiveStrand_);
    receivePromise->then(std::bind(&TCPTransport::receiveHandler, this->shared_from_this(), queueElement),
                         std::bind(&TCPTransport::receiveFailureHandler, this->shared_from_this(), std::placeholders::_1, queueElement));
    tcpEndpoint_->receive(common::DataBuffer(receiveData_), std::move(receivePromise));
}

void TCPTransport::receiveHandler(ReceiveQueue::iterator queueElement)
{
    queueElement->second->resolve(std::move(receiveData_));
    receiveQueue_.erase(queueElement);

    if(!receiveQueue_.empty())
    {
        this->doReceive();
    }
}

void TCPTransport::receiveFailureHandler(const aasdk::error::Error& e, ReceiveQueue::iterator queueElement)
{
    queueElement->second->reject(e);
    receiveQueue_.erase(queueElement);

    if(!receiveQueue_.empty())
    {
        this->doReceive();
    }
}

void TCPTransport::doSend()
{
    auto queueElement = sendQueue_.begin();

    auto sendPromise = tcp::ITCPEndpoint::Promise::defer(sendStrand_);
    sendPromise->then(std::bind(&TCPTransport::sendHandler, this->shared_from_this(), queueElement),
                      std::bind(&TCPTransport::sendFailureHandler, this->shared_from_this(), std::placeholders::_1, queueElement));
    tcpEndpoint_->send(common::DataConstBuffer(queueElement->first), std::move(sendPromise));
}

void TCPTransport::sendHandler(SendQueue::iterator queueElement)
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

}
}
}
