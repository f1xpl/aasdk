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

#pragma once

#include <gmock/gmock.h>
#include <f1x/aasdk/TCP/ITCPWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace tcp
{
namespace ut
{

class TCPWrapperMock: public ITCPWrapper
{
public:
    MOCK_METHOD3(asyncWrite, void(boost::asio::ip::tcp::socket& socket, common::DataConstBuffer buffer, Handler handler));
    MOCK_METHOD3(asyncRead, void(boost::asio::ip::tcp::socket& socket, common::DataBuffer buffer, Handler handler));
    MOCK_METHOD1(close, void(boost::asio::ip::tcp::socket& socket));
    MOCK_METHOD4(asyncConnect, void(boost::asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port, ConnectHandler handler));
    MOCK_METHOD3(connect, boost::system::error_code(boost::asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port));
};

}
}
}
}
