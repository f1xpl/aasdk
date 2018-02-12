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

#include <f1x/aasdk/Messenger/MessageId.hpp>
#include <f1x/aasdk/Messenger/Timestamp.hpp>
#include <f1x/aasdk/Channel/ServiceChannel.hpp>
#include <f1x/aasdk/Channel/AV/IAVInputServiceChannel.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace av
{

class AVInputServiceChannel: public IAVInputServiceChannel, public ServiceChannel, public std::enable_shared_from_this<AVInputServiceChannel>
{
public:
    AVInputServiceChannel(boost::asio::io_service::strand& strand, messenger::IMessenger::Pointer messenger);

    void receive(IAVInputServiceChannelEventHandler::Pointer eventHandler) override;
    void sendChannelOpenResponse(const proto::messages::ChannelOpenResponse& response, SendPromise::Pointer promise) override;
    void sendAVChannelSetupResponse(const proto::messages::AVChannelSetupResponse& response, SendPromise::Pointer promise) override;
    void sendAVInputOpenResponse(const proto::messages::AVInputOpenResponse& response, SendPromise::Pointer promise) override;
    void sendAVMediaWithTimestampIndication(messenger::Timestamp::ValueType, const common::Data& data, SendPromise::Pointer promise) override;
    messenger::ChannelId getId() const override;

private:
    using std::enable_shared_from_this<AVInputServiceChannel>::shared_from_this;
    void messageHandler(messenger::Message::Pointer message, IAVInputServiceChannelEventHandler::Pointer eventHandler);
    void handleAVChannelSetupRequest(const common::DataConstBuffer& payload, IAVInputServiceChannelEventHandler::Pointer eventHandler);
    void handleAVInputOpenRequest(const common::DataConstBuffer& payload, IAVInputServiceChannelEventHandler::Pointer eventHandler);
    void handleAVMediaAckIndication(const common::DataConstBuffer& payload, IAVInputServiceChannelEventHandler::Pointer eventHandler);
    void handleChannelOpenRequest(const common::DataConstBuffer& payload, IAVInputServiceChannelEventHandler::Pointer eventHandler);
};

}
}
}
}
