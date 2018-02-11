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

#include <boost/asio.hpp>
#include <f1x/aasdk/Messenger/IMessenger.hpp>
#include <f1x/aasdk/Channel/Promise.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{

class ServiceChannel
{
protected:
    ServiceChannel(boost::asio::io_service::strand& strand,
                   messenger::IMessenger::Pointer messenger,
                   messenger::ChannelId channelId);

    virtual ~ServiceChannel() = default;
    void send(messenger::Message::Pointer message, SendPromise::Pointer promise);

    boost::asio::io_service::strand& strand_;
    messenger::IMessenger::Pointer messenger_;
    messenger::ChannelId channelId_;
};

}
}
}
