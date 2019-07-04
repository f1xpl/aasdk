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

#include <aasdk_proto/ControlMessageIdsEnum.pb.h>
#include <aasdk_proto/AVChannelMessageIdsEnum.pb.h>
#include <f1x/aasdk/Messenger/Timestamp.hpp>
#include <f1x/aasdk/Channel/AV/IVideoServiceChannelEventHandler.hpp>
#include <f1x/aasdk/Channel/AV/VideoServiceChannel.hpp>
#include <f1x/aasdk/Common/Log.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace av
{

VideoServiceChannel::VideoServiceChannel(boost::asio::io_service::strand& strand, messenger::IMessenger::Pointer messenger)
    : ServiceChannel(strand, std::move(messenger), messenger::ChannelId::VIDEO)
{

}

void VideoServiceChannel::receive(IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    auto receivePromise = messenger::ReceivePromise::defer(strand_);
    receivePromise->then(std::bind(&VideoServiceChannel::messageHandler, this->shared_from_this(), std::placeholders::_1, eventHandler),
                        std::bind(&IVideoServiceChannelEventHandler::onChannelError, eventHandler, std::placeholders::_1));

    messenger_->enqueueReceive(channelId_, std::move(receivePromise));
}

messenger::ChannelId VideoServiceChannel::getId() const
{
    return channelId_;
}

void VideoServiceChannel::sendChannelOpenResponse(const proto::messages::ChannelOpenResponse& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::CONTROL));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::CHANNEL_OPEN_RESPONSE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void VideoServiceChannel::sendAVChannelSetupResponse(const proto::messages::AVChannelSetupResponse& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::AVChannelMessage::SETUP_RESPONSE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void VideoServiceChannel::sendVideoFocusIndication(const proto::messages::VideoFocusIndication& indication, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::AVChannelMessage::VIDEO_FOCUS_INDICATION).getData());
    message->insertPayload(indication);

    this->send(std::move(message), std::move(promise));
}

void VideoServiceChannel::sendAVMediaAckIndication(const proto::messages::AVMediaAckIndication& indication, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::AVChannelMessage::AV_MEDIA_ACK_INDICATION).getData());
    message->insertPayload(indication);

    this->send(std::move(message), std::move(promise));
}

void VideoServiceChannel::messageHandler(messenger::Message::Pointer message, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    messenger::MessageId messageId(message->getPayload());
    common::DataConstBuffer payload(message->getPayload(), messageId.getSizeOf());

    switch(messageId.getId())
    {
    case proto::ids::AVChannelMessage::SETUP_REQUEST:
        this->handleAVChannelSetupRequest(payload, std::move(eventHandler));
        break;
    case proto::ids::AVChannelMessage::START_INDICATION:
        this->handleStartIndication(payload, std::move(eventHandler));
        break;
    case proto::ids::AVChannelMessage::STOP_INDICATION:
        this->handleStopIndication(payload, std::move(eventHandler));
        break;
    case proto::ids::AVChannelMessage::AV_MEDIA_WITH_TIMESTAMP_INDICATION:
        this->handleAVMediaWithTimestampIndication(payload, std::move(eventHandler));
        break;
    case proto::ids::AVChannelMessage::AV_MEDIA_INDICATION:
        eventHandler->onAVMediaIndication(payload);
        break;
    case proto::ids::ControlMessage::CHANNEL_OPEN_REQUEST:
        this->handleChannelOpenRequest(payload, std::move(eventHandler));
        break;
    case proto::ids::AVChannelMessage::VIDEO_FOCUS_REQUEST:
        this->handleVideoFocusRequest(payload, std::move(eventHandler));
        break;
    default:
        AASDK_LOG(error) << "[VideoServiceChannel] message not handled: " << messageId.getId();
        this->receive(std::move(eventHandler));
        break;
    }
}

void VideoServiceChannel::handleAVChannelSetupRequest(const common::DataConstBuffer& payload, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::AVChannelSetupRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onAVChannelSetupRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void VideoServiceChannel::handleStartIndication(const common::DataConstBuffer& payload, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::AVChannelStartIndication indication;
    if(indication.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onAVChannelStartIndication(indication);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void VideoServiceChannel::handleStopIndication(const common::DataConstBuffer& payload, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::AVChannelStopIndication indication;
    if(indication.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onAVChannelStopIndication(indication);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void VideoServiceChannel::handleChannelOpenRequest(const common::DataConstBuffer& payload, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::ChannelOpenRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onChannelOpenRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void VideoServiceChannel::handleVideoFocusRequest(const common::DataConstBuffer& payload, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::VideoFocusRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onVideoFocusRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void VideoServiceChannel::handleAVMediaWithTimestampIndication(const common::DataConstBuffer& payload, IVideoServiceChannelEventHandler::Pointer eventHandler)
{
    if(payload.size >= sizeof(messenger::Timestamp::ValueType))
    {
        messenger::Timestamp timestamp(payload);
        eventHandler->onAVMediaWithTimestampIndication(timestamp.getValue(), common::DataConstBuffer(payload.cdata, payload.size, sizeof(messenger::Timestamp::ValueType)));
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

}
}
}
}
