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
#include <f1x/aasdk/Channel/ServiceChannel.hpp>
#include <f1x/aasdk/Channel/Control/IControlServiceChannel.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace control
{

class ControlServiceChannel: public IControlServiceChannel, public ServiceChannel, public std::enable_shared_from_this<ControlServiceChannel>
{
public:
    ControlServiceChannel(boost::asio::io_service::strand& strand, messenger::IMessenger::Pointer messenger);

    void receive(IControlServiceChannelEventHandler::Pointer eventHandler) override;

    void sendVersionRequest(SendPromise::Pointer promise) override;
    void sendHandshake(common::Data handshakeBuffer, SendPromise::Pointer promise) override;
    void sendAuthComplete(const proto::messages::AuthCompleteIndication& response, SendPromise::Pointer promise) override;
    void sendServiceDiscoveryResponse(const proto::messages::ServiceDiscoveryResponse& response, SendPromise::Pointer promise) override;
    void sendAudioFocusResponse(const proto::messages::AudioFocusResponse& response, SendPromise::Pointer promise) override;
    void sendShutdownRequest(const proto::messages::ShutdownRequest& request, SendPromise::Pointer promise) override;
    void sendShutdownResponse(const proto::messages::ShutdownResponse& response, SendPromise::Pointer promise) override;
    void sendNavigationFocusResponse(const proto::messages::NavigationFocusResponse& respons, SendPromise::Pointer promisee) override;
    void sendPingRequest(const proto::messages::PingRequest& request, SendPromise::Pointer promise) override;

private:
    using std::enable_shared_from_this<ControlServiceChannel>::shared_from_this;
    void messageHandler(messenger::Message::Pointer message, IControlServiceChannelEventHandler::Pointer eventHandler);

    void handleVersionResponse(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
    void handleServiceDiscoveryRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
    void handleAudioFocusRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
    void handleShutdownRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
    void handleShutdownResponse(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
    void handleNavigationFocusRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
    void handlePingResponse(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler);
};

}
}
}
}
