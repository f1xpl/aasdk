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

#include <boost/test/unit_test.hpp>
#include <f1x/aasdk/Transport/UT/Transport.mock.hpp>
#include <f1x/aasdk/Messenger/UT/Cryptor.mock.hpp>
#include <f1x/aasdk/Messenger/UT/SendPromiseHandler.mock.hpp>
#include <f1x/aasdk/Messenger/Promise.hpp>
#include <f1x/aasdk/Messenger/MessageOutStream.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{
namespace ut
{

using ::testing::_;
using ::testing::SaveArg;
using ::testing::SetArgReferee;
using ::testing::Return;

class MessageOutStreamUnitTest
{
protected:
    MessageOutStreamUnitTest()
        : transport_(&transportMock_, [](auto*) {})
        , cryptor_(&cryptorMock_, [](auto*) {})
        , sendPromise_(SendPromise::defer(ioService_))
    {
        sendPromise_->then(std::bind(&SendPromiseHandlerMock::onResolve, &sendPromiseHandlerMock_),
                          std::bind(&SendPromiseHandlerMock::onReject, &sendPromiseHandlerMock_, std::placeholders::_1));
    }

    boost::asio::io_service ioService_;
    transport::ut::TransportMock transportMock_;
    transport::ITransport::Pointer transport_;
    CryptorMock cryptorMock_;
    ICryptor::Pointer cryptor_;
    SendPromiseHandlerMock sendPromiseHandlerMock_;
    SendPromise::Pointer sendPromise_;
};

ACTION(ThrowSSLWriteException)
{
    throw error::Error(error::ErrorCode::SSL_WRITE, 32);
}

BOOST_FIXTURE_TEST_CASE(MessageOutStream_SendPlainMessage, MessageOutStreamUnitTest)
{
    const FrameHeader frameHeader(ChannelId::INPUT, FrameType::BULK, EncryptionType::PLAIN, MessageType::CONTROL);
    const common::Data payload(1000, 0x5E);
    const FrameSize frameSize(payload.size());

    const auto& frameHeaderData = frameHeader.getData();
    common::Data expectedData(frameHeaderData.begin(), frameHeaderData.end());

    const auto& frameSizeData = frameSize.getData();
    expectedData.insert(expectedData.end(), frameSizeData.begin(), frameSizeData.end());
    expectedData.insert(expectedData.end(), payload.begin(), payload.end());

    transport::ITransport::SendPromise::Pointer transportSendPromise;
    EXPECT_CALL(transportMock_, send(expectedData, _)).WillOnce(SaveArg<1>(&transportSendPromise));

    Message::Pointer message(std::make_shared<Message>(ChannelId::INPUT, EncryptionType::PLAIN, MessageType::CONTROL));
    message->insertPayload(payload);
    MessageOutStream::Pointer messageOutStream(std::make_shared<MessageOutStream>(ioService_, transport_, cryptor_));
    messageOutStream->stream(message, std::move(sendPromise_));

    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(sendPromiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve());
    transportSendPromise->resolve();
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(MessageOutStream_SendEncryptedMessage, MessageOutStreamUnitTest)
{
    const FrameHeader frameHeader(ChannelId::VIDEO, FrameType::BULK, EncryptionType::ENCRYPTED, MessageType::CONTROL);
    const common::Data encryptedPayload(2000, 0x5F);
    const FrameSize frameSize(encryptedPayload.size());

    const auto& frameHeaderData = frameHeader.getData();
    common::Data expectedData(frameHeaderData.begin(), frameHeaderData.end());

    const auto& frameSizeData = frameSize.getData();
    expectedData.insert(expectedData.end(), frameSizeData.begin(), frameSizeData.end());
    expectedData.insert(expectedData.end(), encryptedPayload.begin(), encryptedPayload.end());

    common::Data encryptedData(expectedData.begin(), expectedData.begin() + FrameHeader::getSizeOf() + FrameSize::getSizeOf(FrameSizeType::SHORT));
    encryptedData.insert(encryptedData.end(), encryptedPayload.begin(), encryptedPayload.end());
    transport::ITransport::SendPromise::Pointer transportSendPromise;
    EXPECT_CALL(cryptorMock_, encrypt(_, _)).WillOnce(testing::DoAll(SetArgReferee<0>(encryptedData), Return(encryptedPayload.size())));
    EXPECT_CALL(transportMock_, send(expectedData, _)).WillOnce(SaveArg<1>(&transportSendPromise));

    Message::Pointer message(std::make_shared<Message>(ChannelId::VIDEO, EncryptionType::ENCRYPTED, MessageType::CONTROL));
    const common::Data payload(1000, 0x5E);
    message->insertPayload(payload);
    MessageOutStream::Pointer messageOutStream(std::make_shared<MessageOutStream>(ioService_, transport_, cryptor_));
    messageOutStream->stream(message, std::move(sendPromise_));

    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(sendPromiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve());
    transportSendPromise->resolve();
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(MessageOutStream_MessageEncryptionFailed, MessageOutStreamUnitTest)
{
    Message::Pointer message(std::make_shared<Message>(ChannelId::VIDEO, EncryptionType::ENCRYPTED, MessageType::CONTROL));
    const common::Data payload(1000, 0x5E);
    message->insertPayload(payload);
    MessageOutStream::Pointer messageOutStream(std::make_shared<MessageOutStream>(ioService_, transport_, cryptor_));

    EXPECT_CALL(cryptorMock_, encrypt(_, _)).WillOnce(ThrowSSLWriteException());
    EXPECT_CALL(sendPromiseHandlerMock_, onReject(error::Error(error::ErrorCode::SSL_WRITE, 32)));
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve()).Times(0);
    messageOutStream->stream(message, std::move(sendPromise_));

    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(MessageOutStream_SendError, MessageOutStreamUnitTest)
{
    Message::Pointer message(std::make_shared<Message>(ChannelId::VIDEO, EncryptionType::PLAIN, MessageType::CONTROL));
    const common::Data payload(1000, 0x5E);
    message->insertPayload(payload);
    MessageOutStream::Pointer messageOutStream(std::make_shared<MessageOutStream>(ioService_, transport_, cryptor_));

    transport::ITransport::SendPromise::Pointer transportSendPromise;
    EXPECT_CALL(transportMock_, send(_, _)).WillOnce(SaveArg<1>(&transportSendPromise));
    messageOutStream->stream(message, std::move(sendPromise_));

    ioService_.run();
    ioService_.reset();

    const error::Error e(error::ErrorCode::USB_TRANSFER, 513);
    EXPECT_CALL(sendPromiseHandlerMock_, onReject(e));
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve()).Times(0);

    transportSendPromise->reject(e);
    ioService_.run();
}

BOOST_FIXTURE_TEST_CASE(MessageOutStream_SendSplittedMessage, MessageOutStreamUnitTest)
{
    const size_t maxFramePayloadSize = 0x4000;

    const common::Data frame1Payload(maxFramePayloadSize, 0x5E);
    const common::Data frame2Payload(maxFramePayloadSize, 0x5E);

    const FrameHeader frame1Header(ChannelId::VIDEO, FrameType::FIRST, EncryptionType::PLAIN, MessageType::CONTROL);
    const auto& frame1HeaderData = frame1Header.getData();

    const FrameSize frame1Size(frame1Payload.size(), frame1Payload.size() + frame2Payload.size());
    const auto& frame1SizeData = frame1Size.getData();

    const FrameHeader frame2Header(ChannelId::VIDEO, FrameType::LAST, EncryptionType::PLAIN, MessageType::CONTROL);
    const auto& frame2HeaderData = frame2Header.getData();

    const FrameSize frame2Size(frame2Payload.size());
    const auto& frame2SizeData = frame2Size.getData();

    Message::Pointer message(std::make_shared<Message>(ChannelId::VIDEO, EncryptionType::PLAIN, MessageType::CONTROL));
    message->insertPayload(frame1Payload);
    message->insertPayload(frame2Payload);

    transport::ITransport::SendPromise::Pointer transportSendPromise;
    common::Data expectedData1(frame1HeaderData.begin(), frame1HeaderData.end());
    expectedData1.insert(expectedData1.end(), frame1SizeData.begin(), frame1SizeData.end());
    expectedData1.insert(expectedData1.end(), frame1Payload.begin(), frame1Payload.end());
    EXPECT_CALL(transportMock_, send(expectedData1, _)).WillOnce(SaveArg<1>(&transportSendPromise));

    MessageOutStream::Pointer messageOutStream(std::make_shared<MessageOutStream>(ioService_, transport_, cryptor_));
    messageOutStream->stream(message, std::move(sendPromise_));

    ioService_.run();
    ioService_.reset();

    common::Data expectedData2(frame2HeaderData.begin(), frame2HeaderData.end());
    expectedData2.insert(expectedData2.end(), frame2SizeData.begin(), frame2SizeData.end());
    expectedData2.insert(expectedData2.end(), frame2Payload.begin(), frame2Payload.end());
    EXPECT_CALL(transportMock_, send(expectedData2, _)).WillOnce(SaveArg<1>(&transportSendPromise));

    auto secondSendPromise = SendPromise::defer(ioService_);
    SendPromiseHandlerMock secondSendPromiseHandlerMock;
    secondSendPromise->then(std::bind(&SendPromiseHandlerMock::onResolve, &secondSendPromiseHandlerMock),
                           std::bind(&SendPromiseHandlerMock::onReject, &secondSendPromiseHandlerMock, std::placeholders::_1));

    EXPECT_CALL(secondSendPromiseHandlerMock, onResolve()).Times(0);
    EXPECT_CALL(secondSendPromiseHandlerMock, onReject(error::Error(error::ErrorCode::OPERATION_IN_PROGRESS)));
    messageOutStream->stream(message, std::move(secondSendPromise));

    transportSendPromise->resolve();
    ioService_.run();
    ioService_.reset();

    EXPECT_CALL(sendPromiseHandlerMock_, onReject(_)).Times(0);
    EXPECT_CALL(sendPromiseHandlerMock_, onResolve());
    transportSendPromise->resolve();
    ioService_.run();
}

}
}
}
}
