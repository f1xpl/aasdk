#include <f1x/aasdk/TCP/TCPEndpoint.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{

TCPEndpoint::TCPEndpoint(ITCPWrapper& tcpWrapper, boost::asio::ip::tcp::socket socket)
    : tcpWrapper_(tcpWrapper)
    , socket_(std::move(socket))
{

}

void TCPEndpoint::send(common::DataConstBuffer buffer, Promise::Pointer promise)
{
    tcpWrapper_.asyncWrite(socket_, std::move(buffer),
                           std::bind(&TCPEndpoint::asyncOperationHandler,
                                     this->shared_from_this(),
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::move(promise)));
}

void TCPEndpoint::receive(common::DataBuffer buffer, Promise::Pointer promise)
{
    tcpWrapper_.asyncRead(socket_, std::move(buffer),
                          std::bind(&TCPEndpoint::asyncOperationHandler,
                                    this->shared_from_this(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::move(promise)));
}

void TCPEndpoint::stop()
{
    try
    {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket_.close();
    }
    catch(const boost::system::system_error&)
    {

    }
}

void TCPEndpoint::asyncOperationHandler(const boost::system::error_code& ec, size_t bytesTransferred, Promise::Pointer promise)
{
    if(!ec)
    {
        promise->resolve(bytesTransferred);
    }
    else
    {
        promise->reject(error::Error(error::ErrorCode::TCP_TRANSFER, static_cast<uint32_t>(ec.value())));
    }
}

}
}
}
