#pragma once

#include <f1x/aasdk/TCP/ITCPWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{

class TCPWrapper: public ITCPWrapper
{
public:
    void asyncWrite(boost::asio::ip::tcp::socket& socket, common::DataConstBuffer buffer, Handler handler) override;
    void asyncRead(boost::asio::ip::tcp::socket& socket, common::DataBuffer buffer, Handler handler) override;
    void close(boost::asio::ip::tcp::socket& socket) override;
    void asyncConnect(boost::asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port, ConnectHandler handler) override;
};

}
}
}
