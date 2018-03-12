#pragma once

#include <list>
#include <f1x/aasdk/TCP/ITCPEndpoint.hpp>
#include <f1x/aasdk/Transport/ITransport.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

class TCPTransport: public ITransport, public std::enable_shared_from_this<TCPTransport>, boost::noncopyable
{
public:
    TCPTransport(boost::asio::io_service& ioService, tcp::ITCPEndpoint::Pointer tcpEndpoint);

    void receive(size_t size, ReceivePromise::Pointer promise) override;
    void send(common::Data data, SendPromise::Pointer promise) override;
    void stop() override;

private:
    typedef std::list<std::pair<size_t, ReceivePromise::Pointer>> ReceiveQueue;
    typedef std::list<std::pair<common::Data, SendPromise::Pointer>> SendQueue;

    using std::enable_shared_from_this<TCPTransport>::shared_from_this;
    void doReceive();
    void receiveHandler(ReceiveQueue::iterator queueElement);
    void receiveFailureHandler(const aasdk::error::Error& e, ReceiveQueue::iterator queueElement);

    void doSend();
    void sendHandler(SendQueue::iterator queueElement);
    void sendFailureHandler(const aasdk::error::Error& e, SendQueue::iterator queueElement);

    boost::asio::io_service& ioService_;
    tcp::ITCPEndpoint::Pointer tcpEndpoint_;

    boost::asio::io_service::strand receiveStrand_;
    ReceiveQueue receiveQueue_;
    common::Data receiveData_;

    boost::asio::io_service::strand sendStrand_;
    SendQueue sendQueue_;
};

}
}
}
