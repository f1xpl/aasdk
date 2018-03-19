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
    TCPEndpoint(ITCPWrapper& tcpWrapper, SocketPointer socket);

    void send(common::DataConstBuffer buffer, Promise::Pointer promise) override;
    void receive(common::DataBuffer buffer, Promise::Pointer promise) override;
    void stop() override;

private:
    using std::enable_shared_from_this<TCPEndpoint>::shared_from_this;

    void asyncOperationHandler(const boost::system::error_code& ec, size_t bytesTransferred, Promise::Pointer promise);

    ITCPWrapper& tcpWrapper_;
    SocketPointer socket_;
};

}
}
}
