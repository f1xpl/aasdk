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

#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/aasdk/Transport/ITransport.hpp>
#include <f1x/aasdk/Messenger/ICryptor.hpp>
#include <f1x/aasdk/Messenger/IMessageOutStream.hpp>
#include <f1x/aasdk/Messenger/FrameHeader.hpp>
#include <f1x/aasdk/Messenger/FrameSize.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class MessageOutStream: public IMessageOutStream, public std::enable_shared_from_this<MessageOutStream>
{
public:
    MessageOutStream(boost::asio::io_service& ioService, transport::ITransport::Pointer transport, ICryptor::Pointer cryptor);

    void stream(Message::Pointer message, SendPromise::Pointer promise) override;

private:
    using std::enable_shared_from_this<MessageOutStream>::shared_from_this;

    void streamSplittedMessage();
    common::Data compoundFrame(FrameType frameType, const common::DataConstBuffer& payloadBuffer);
    void streamEncryptedFrame(FrameType frameType, const common::DataConstBuffer& payloadBuffer);
    void streamPlainFrame(FrameType frameType, const common::DataConstBuffer& payloadBuffer);
    void setFrameSize(common::Data& data, FrameType frameType, size_t payloadSize, size_t totalSize);
    void reset();

    boost::asio::io_service::strand strand_;
    transport::ITransport::Pointer transport_;
    ICryptor::Pointer cryptor_;
    Message::Pointer message_;
    size_t offset_;
    size_t remainingSize_;
    SendPromise::Pointer promise_;

    static constexpr size_t cMaxFramePayloadSize = 0x4000;

    MessageOutStream(const MessageOutStream&) = delete;
};

}
}
}
