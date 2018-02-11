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

#include <memory>
#include <functional>
#include <f1x/aasdk/Messenger/ICryptor.hpp>
#include <f1x/aasdk/Messenger/Message.hpp>
#include <f1x/aasdk/Messenger/Promise.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class IMessenger
{
public:
    IMessenger() = default;
    virtual ~IMessenger() = default;

    typedef std::shared_ptr<IMessenger> Pointer;

    virtual void enqueueReceive(ChannelId channelId, ReceivePromise::Pointer promise) = 0;
    virtual void enqueueSend(Message::Pointer message, SendPromise::Pointer promise) = 0;
    virtual void stop() = 0;
};

}
}
}
