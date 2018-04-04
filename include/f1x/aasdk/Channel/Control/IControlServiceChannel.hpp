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

#include <aasdk_proto/ShutdownRequestMessage.pb.h>
#include <aasdk_proto/ShutdownResponseMessage.pb.h>
#include <aasdk_proto/AuthCompleteIndicationMessage.pb.h>
#include <aasdk_proto/ServiceDiscoveryResponseMessage.pb.h>
#include <aasdk_proto/AudioFocusResponseMessage.pb.h>
#include <aasdk_proto/NavigationFocusResponseMessage.pb.h>
#include <aasdk_proto/StatusEnum.pb.h>
#include <aasdk_proto/PingRequestMessage.pb.h>
#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/aasdk/Channel/Promise.hpp>
#include <f1x/aasdk/Channel/Control/IControlServiceChannelEventHandler.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace control
{

class IControlServiceChannel
{
public:
    typedef std::shared_ptr<IControlServiceChannel> Pointer;

    IControlServiceChannel() = default;
    virtual ~IControlServiceChannel() = default;

    virtual void receive(IControlServiceChannelEventHandler::Pointer eventHandler) = 0;

    virtual void sendVersionRequest(SendPromise::Pointer promise) = 0;
    virtual void sendHandshake(common::Data handshakeBuffer, SendPromise::Pointer promise) = 0;
    virtual void sendAuthComplete(const proto::messages::AuthCompleteIndication& response, SendPromise::Pointer promise) = 0;
    virtual void sendServiceDiscoveryResponse(const proto::messages::ServiceDiscoveryResponse& response, SendPromise::Pointer promise) = 0;
    virtual void sendAudioFocusResponse(const proto::messages::AudioFocusResponse& response, SendPromise::Pointer promise) = 0;
    virtual void sendShutdownRequest(const proto::messages::ShutdownRequest& request, SendPromise::Pointer promise) = 0;
    virtual void sendShutdownResponse(const proto::messages::ShutdownResponse& response, SendPromise::Pointer promise) = 0;
    virtual void sendNavigationFocusResponse(const proto::messages::NavigationFocusResponse& response, SendPromise::Pointer promise) = 0;
    virtual void sendPingRequest(const proto::messages::PingRequest& request, SendPromise::Pointer promise) = 0;
};

}
}
}
}
