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
#include <list>
#include <f1x/aasdk/Messenger/IMessenger.hpp>
#include <f1x/aasdk/Messenger/IMessageInStream.hpp>
#include <f1x/aasdk/Messenger/IMessageOutStream.hpp>
#include <f1x/aasdk/Messenger/ChannelReceiveMessageQueue.hpp>
#include <f1x/aasdk/Messenger/ChannelReceivePromiseQueue.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class Messenger: public IMessenger, public std::enable_shared_from_this<Messenger>
{
public:
    Messenger(boost::asio::io_service& ioService, IMessageInStream::Pointer messageInStream, IMessageOutStream::Pointer messageOutStream);
    void enqueueReceive(ChannelId channelId, ReceivePromise::Pointer promise) override;
    void enqueueSend(Message::Pointer message, SendPromise::Pointer promise) override;
    void stop() override;

private:
    using std::enable_shared_from_this<Messenger>::shared_from_this;
    typedef std::list<std::pair<Message::Pointer, SendPromise::Pointer>> ChannelSendQueue;
    void doSend();
    void inStreamMessageHandler(Message::Pointer message);
    void outStreamMessageHandler(ChannelSendQueue::iterator queueElement);
    void rejectReceivePromiseQueue(const error::Error& e);
    void rejectSendPromiseQueue(const error::Error& e);

    boost::asio::io_service::strand receiveStrand_;
    boost::asio::io_service::strand sendStrand_;
    IMessageInStream::Pointer messageInStream_;
    IMessageOutStream::Pointer messageOutStream_;

    ChannelReceivePromiseQueue channelReceivePromiseQueue_;
    ChannelReceiveMessageQueue channelReceiveMessageQueue_;
    ChannelSendQueue channelSendPromiseQueue_;

    Messenger(const Messenger&) = delete;
};

}
}
}
