#include <boost/asio.hpp>
#include <f1x/aasdk/TCP/TCPWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{

void TCPWrapper::asyncWrite(boost::asio::ip::tcp::socket& socket, common::DataConstBuffer buffer, Handler handler)
{
    boost::asio::async_write(socket, boost::asio::buffer(buffer.cdata, buffer.size), std::move(handler));
}

void TCPWrapper::asyncRead(boost::asio::ip::tcp::socket& socket, common::DataBuffer buffer, Handler handler)
{
    boost::asio::async_read(socket, boost::asio::buffer(buffer.data, buffer.size), std::move(handler));
}

}
}
}
