#pragma once

#include <functional>
#include <boost/asio/ip/tcp.hpp>
#include <f1x/aasdk/Common/Data.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{

class ITCPWrapper
{
public:
    typedef std::function<void(const boost::system::error_code&, size_t)> Handler;

    virtual ~ITCPWrapper() = default;

    virtual void asyncWrite(boost::asio::ip::tcp::socket& socket, common::DataConstBuffer buffer, Handler handler) = 0;
    virtual void asyncRead(boost::asio::ip::tcp::socket& socket, common::DataBuffer buffer, Handler handler) = 0;
};

}
}
}
