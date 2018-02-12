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
#include <aasdk_proto/StatusEnum.pb.h>
#include <aasdk_proto/BindingResponseMessage.pb.h>
#include <aasdk_proto/InputEventIndicationMessage.pb.h>
#include <aasdk_proto/ChannelOpenResponseMessage.pb.h>
#include <f1x/aasdk/Messenger/ChannelId.hpp>
#include <f1x/aasdk/Channel/Promise.hpp>
#include <f1x/aasdk/Channel/Input/IInputServiceChannelEventHandler.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace input
{

class IInputServiceChannel
{
public:
    typedef std::shared_ptr<IInputServiceChannel> Pointer;

    IInputServiceChannel() = default;
    virtual ~IInputServiceChannel() = default;

    virtual void receive(IInputServiceChannelEventHandler::Pointer eventHandler) = 0;
    virtual void sendChannelOpenResponse(const proto::messages::ChannelOpenResponse& response, SendPromise::Pointer promise) = 0;
    virtual void sendInputEventIndication(const proto::messages::InputEventIndication& indication, SendPromise::Pointer promise) = 0;
    virtual void sendBindingResponse(const proto::messages::BindingResponse& response, SendPromise::Pointer promise) = 0;
    virtual messenger::ChannelId getId() const = 0;
};

}
}
}
}
