#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <f1x/aasdk/TCP/ITCPEndpoint.hpp>
#include <f1x/aasdk/TCP/ITCPWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{

class TCPEndpoint: public ITCPEndpoint, public std::enable_shared_from_this<TCPEndpoint>
{
public:
    TCPEndpoint(ITCPWrapper& tcpWrapper, boost::asio::ip::tcp::socket socket);

    void send(common::DataConstBuffer buffer, Promise::Pointer promise) override;
    void receive(common::DataBuffer buffer, Promise::Pointer promise) override;
    void stop() override;

private:
    using std::enable_shared_from_this<TCPEndpoint>::shared_from_this;

    void asyncOperationHandler(const boost::system::error_code& ec, size_t bytesTransferred, Promise::Pointer promise);

    ITCPWrapper& tcpWrapper_;
    boost::asio::ip::tcp::socket socket_;
};

}
}
}
