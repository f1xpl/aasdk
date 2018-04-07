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

#include <boost/endian/conversion.hpp>
#include <aasdk_proto/ControlMessageIdsEnum.pb.h>
#include <f1x/aasdk/Version.hpp>
#include <f1x/aasdk/IO/PromiseLink.hpp>
#include <f1x/aasdk/Channel/Control/ControlServiceChannel.hpp>
#include <f1x/aasdk/Channel/Control/IControlServiceChannelEventHandler.hpp>
#include <f1x/aasdk/Common/Log.hpp>

namespace f1x
{
namespace aasdk
{
namespace channel
{
namespace control
{

ControlServiceChannel::ControlServiceChannel(boost::asio::io_service::strand& strand, messenger::IMessenger::Pointer messenger)
    : ServiceChannel(strand, messenger, messenger::ChannelId::CONTROL)
{

}

void ControlServiceChannel::sendVersionRequest(SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::PLAIN, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::VERSION_REQUEST).getData());

    common::Data versionBuffer(4, 0);
    reinterpret_cast<uint16_t&>(versionBuffer[0]) = boost::endian::native_to_big(AASDK_MAJOR);
    reinterpret_cast<uint16_t&>(versionBuffer[2]) = boost::endian::native_to_big(AASDK_MINOR);
    message->insertPayload(versionBuffer);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendHandshake(common::Data handshakeBuffer, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::PLAIN, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::SSL_HANDSHAKE).getData());
    message->insertPayload(handshakeBuffer);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendAuthComplete(const proto::messages::AuthCompleteIndication& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::PLAIN, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::AUTH_COMPLETE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendServiceDiscoveryResponse(const proto::messages::ServiceDiscoveryResponse& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::SERVICE_DISCOVERY_RESPONSE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendAudioFocusResponse(const proto::messages::AudioFocusResponse& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::AUDIO_FOCUS_RESPONSE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendShutdownRequest(const proto::messages::ShutdownRequest& request, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::SHUTDOWN_REQUEST).getData());
    message->insertPayload(request);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendShutdownResponse(const proto::messages::ShutdownResponse& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::SHUTDOWN_RESPONSE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendNavigationFocusResponse(const proto::messages::NavigationFocusResponse& response, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::NAVIGATION_FOCUS_RESPONSE).getData());
    message->insertPayload(response);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::sendPingRequest(const proto::messages::PingRequest& request, SendPromise::Pointer promise)
{
    auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::PLAIN, messenger::MessageType::SPECIFIC));
    message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::PING_REQUEST).getData());
    message->insertPayload(request);

    this->send(std::move(message), std::move(promise));
}

void ControlServiceChannel::receive(IControlServiceChannelEventHandler::Pointer eventHandler)
{
    auto receivePromise  = messenger::ReceivePromise::defer(strand_);
    receivePromise->then(std::bind(&ControlServiceChannel::messageHandler, this->shared_from_this(), std::placeholders::_1, eventHandler),
                        std::bind(&IControlServiceChannelEventHandler::onChannelError, eventHandler, std::placeholders::_1));

    messenger_->enqueueReceive(channelId_, std::move(receivePromise));
}

void ControlServiceChannel::messageHandler(messenger::Message::Pointer message, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    messenger::MessageId messageId(message->getPayload());
    common::DataConstBuffer payload(message->getPayload(), messageId.getSizeOf());

    switch(messageId.getId())
    {
    case proto::ids::ControlMessage::VERSION_RESPONSE:
        this->handleVersionResponse(payload, std::move(eventHandler));
        break;
    case proto::ids::ControlMessage::SSL_HANDSHAKE:
        eventHandler->onHandshake(payload);
        break;
    case proto::ids::ControlMessage::SERVICE_DISCOVERY_REQUEST:
        this->handleServiceDiscoveryRequest(payload, std::move(eventHandler));
        break;
    case proto::ids::ControlMessage::AUDIO_FOCUS_REQUEST:
        this->handleAudioFocusRequest(payload, std::move(eventHandler));
        break;
    case proto::ids::ControlMessage::SHUTDOWN_REQUEST:
        this->handleShutdownRequest(payload, std::move(eventHandler));
        break;
    case proto::ids::ControlMessage::SHUTDOWN_RESPONSE:
        this->handleShutdownResponse(payload, std::move(eventHandler));
        break;
    case proto::ids::ControlMessage::NAVIGATION_FOCUS_REQUEST:
        this->handleNavigationFocusRequest(payload, std::move(eventHandler));
        break;
    case proto::ids::ControlMessage::PING_RESPONSE:
        this->handlePingResponse(payload, std::move(eventHandler));
        break;
    default:
        AASDK_LOG(error) << "[ControlServiceChannel] message not handled: " << messageId.getId();
        this->receive(std::move(eventHandler));
        break;
    }
}

void ControlServiceChannel::handleVersionResponse(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    const size_t elements = payload.size / sizeof(uint16_t);
    const uint16_t* versionResponse = reinterpret_cast<const uint16_t*>(payload.cdata);

    uint16_t majorCode = elements > 0 ? boost::endian::big_to_native(versionResponse[0]) : 0;
    uint16_t minorCode = elements > 1 ? boost::endian::big_to_native(versionResponse[1]) : 0;
    proto::enums::VersionResponseStatus::Enum status = elements > 2 ? static_cast<proto::enums::VersionResponseStatus::Enum>(versionResponse[2]) : proto::enums::VersionResponseStatus::MISMATCH;

    eventHandler->onVersionResponse(majorCode, minorCode, status);
}

void ControlServiceChannel::handleServiceDiscoveryRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::ServiceDiscoveryRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onServiceDiscoveryRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void ControlServiceChannel::handleAudioFocusRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::AudioFocusRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onAudioFocusRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void ControlServiceChannel::handleShutdownRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::ShutdownRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onShutdownRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void ControlServiceChannel::handleShutdownResponse(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::ShutdownResponse response;
    if(response.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onShutdownResponse(response);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void ControlServiceChannel::handleNavigationFocusRequest(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::NavigationFocusRequest request;
    if(request.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onNavigationFocusRequest(request);
    }
    else
    {
        eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
    }
}

void ControlServiceChannel::handlePingResponse(const common::DataConstBuffer& payload, IControlServiceChannelEventHandler::Pointer eventHandler)
{
    proto::messages::PingResponse response;
    if(response.ParseFromArray(payload.cdata, payload.size))
    {
        eventHandler->onPingResponse(response);
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
