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

#include <f1x/aasdk/Messenger/Message.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

Message::Message(ChannelId channelId, EncryptionType encryptionType, MessageType type)
    : channelId_(channelId)
    , encryptionType_(encryptionType)
    , type_(type)
{
}

Message::Message(Message&& other) noexcept
    : channelId_(other.channelId_)
    , encryptionType_(other.encryptionType_)
    , type_(other.type_)
    , payload_(std::move(other.payload_))
{

}

Message& Message::operator=(Message&& other) noexcept
{
    channelId_ = std::move(other.channelId_);
    encryptionType_ = std::move(other.encryptionType_);
    type_ = std::move(other.type_);
    payload_ = std::move(other.payload_);

    return *this;
}

ChannelId Message::getChannelId() const
{
    return channelId_;
}

EncryptionType Message::getEncryptionType() const
{
    return encryptionType_;
}

MessageType Message::getType() const
{
    return type_;
}

common::Data& Message::getPayload()
{
    return payload_;
}

const common::Data& Message::getPayload() const
{
    return payload_;
}

void Message::insertPayload(const common::Data& payload)
{
    payload_.insert(payload_.end(), payload.begin(), payload.end());
}

void Message::insertPayload(const google::protobuf::Message& message)
{
    auto offset = payload_.size();
    payload_.resize(payload_.size() + message.ByteSize());

    common::DataBuffer buffer(payload_, offset);
    message.SerializeToArray(buffer.data, buffer.size);
}

void Message::insertPayload(const common::DataConstBuffer& buffer)
{
    common::copy(payload_, buffer);
}

void Message::insertPayload(common::DataBuffer& buffer)
{
    common::copy(payload_, buffer);
}

}
}
}
