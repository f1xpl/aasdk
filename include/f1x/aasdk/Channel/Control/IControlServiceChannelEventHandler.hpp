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

#include <f1x/aasdk/Error/Error.hpp>
#include <f1x/aasdk/Common/Data.hpp>
#include <aasdk_proto/ShutdownRequestMessage.pb.h>
#include <aasdk_proto/ShutdownResponseMessage.pb.h>
#include <aasdk_proto/VersionResponseStatusEnum.pb.h>
#include <aasdk_proto/ServiceDiscoveryRequestMessage.pb.h>
#include <aasdk_proto/AudioFocusRequestMessage.pb.h>
#include <aasdk_proto/NavigationFocusRequestMessage.pb.h>
#include <aasdk_proto/PingResponseMessage.pb.h>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace control
{

class IControlServiceChannelEventHandler
{
public:
    typedef std::shared_ptr<IControlServiceChannelEventHandler> Pointer;

    IControlServiceChannelEventHandler() = default;
    virtual ~IControlServiceChannelEventHandler() = default;

    virtual void onVersionResponse(uint16_t majorCode, uint16_t minorCode, proto::enums::VersionResponseStatus::Enum status) = 0;
    virtual void onHandshake(const common::DataConstBuffer& payload) = 0;
    virtual void onServiceDiscoveryRequest(const proto::messages::ServiceDiscoveryRequest& request) = 0;
    virtual void onAudioFocusRequest(const proto::messages::AudioFocusRequest& request) = 0;
    virtual void onShutdownRequest(const proto::messages::ShutdownRequest& request) = 0;
    virtual void onShutdownResponse(const proto::messages::ShutdownResponse& response) = 0;
    virtual void onNavigationFocusRequest(const proto::messages::NavigationFocusRequest& request) = 0;
    virtual void onPingResponse(const proto::messages::PingResponse& response) = 0;
    virtual void onChannelError(const error::Error& e) = 0;
};

}
}
}
}
