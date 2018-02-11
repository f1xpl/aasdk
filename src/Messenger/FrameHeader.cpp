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
#include <f1x/aasdk/Messenger/FrameHeader.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

FrameHeader::FrameHeader(const common::DataConstBuffer& buffer)
{
    channelId_ = static_cast<ChannelId>(buffer.cdata[0]);
    frameType_ = static_cast<FrameType>(buffer.cdata[1] & static_cast<uint32_t>(FrameType::BULK));
    encryptionType_ = static_cast<EncryptionType>(buffer.cdata[1] & static_cast<uint8_t>(EncryptionType::ENCRYPTED));
    messageType_ = static_cast<MessageType>(buffer.cdata[1] & static_cast<uint8_t>(MessageType::CONTROL));
}

FrameHeader::FrameHeader(ChannelId channelId, FrameType frameType, EncryptionType encryptionType, MessageType messageType)
    : channelId_(channelId)
    , frameType_(frameType)
    , encryptionType_(encryptionType)
    , messageType_(messageType)
{

}

ChannelId FrameHeader::getChannelId() const
{
    return channelId_;
}

FrameType FrameHeader::getType() const
{
    return frameType_;
}

EncryptionType FrameHeader::getEncryptionType() const
{
    return encryptionType_;
}

MessageType FrameHeader::getMessageType() const
{
    return messageType_;
}

common::Data FrameHeader::getData() const
{
    common::Data data;

    data.push_back(static_cast<uint8_t>(channelId_));
    data.push_back(static_cast<uint8_t>(encryptionType_) | static_cast<uint8_t>(messageType_) | static_cast<uint8_t>(frameType_));

    return data;
}

}
}
}
