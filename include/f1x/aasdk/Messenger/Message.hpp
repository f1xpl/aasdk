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
#include <google/protobuf/message.h>
#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/aasdk/Messenger/ChannelId.hpp>
#include <f1x/aasdk/Messenger/EncryptionType.hpp>
#include <f1x/aasdk/Messenger/MessageType.hpp>
#include <f1x/aasdk/Messenger/MessageId.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class Message
{
public:
    typedef std::shared_ptr<Message> Pointer;

    Message(ChannelId channelId, EncryptionType encryptionType, MessageType type);
    Message(Message&& other);
    Message& operator=(Message&& other);

    ChannelId getChannelId() const;
    EncryptionType getEncryptionType() const;
    MessageType getType() const;

    common::Data& getPayload();
    const common::Data& getPayload() const;
    void insertPayload(const common::Data& payload);
    void insertPayload(const google::protobuf::Message& message);
    void insertPayload(const common::DataConstBuffer& buffer);
    void insertPayload(common::DataBuffer& buffer);

private:
    ChannelId channelId_;
    EncryptionType encryptionType_;
    MessageType type_;
    common::Data payload_;

    Message(const Message&) = delete;
};

}
}
}
