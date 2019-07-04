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
    socket.async_receive(boost::asio::buffer(buffer.data, buffer.size), std::move(handler));
}

void TCPWrapper::close(boost::asio::ip::tcp::socket& socket)
{
    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket.close(ec);
}

void TCPWrapper::asyncConnect(boost::asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port, ConnectHandler handler)
{
    socket.async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(hostname), port), std::move(handler));
}

boost::system::error_code TCPWrapper::connect(boost::asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port)
{
    boost::system::error_code ec;
    socket.set_option(boost::asio::ip::tcp::no_delay(true), ec);
    socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(hostname), port), ec);
    return ec;
}

}
}
}
