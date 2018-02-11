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

#include <f1x/aasdk/Messenger/ChannelReceiveMessageQueue.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

void ChannelReceiveMessageQueue::push(Message::Pointer message)
{
    const auto channelId = message->getChannelId();

    if(queue_.count(channelId) == 0)
    {
        queue_.emplace(std::make_pair(channelId, MessageQueue()));
    }

    auto& channelQueue = queue_.at(channelId);
    channelQueue.emplace(std::move(message));
}

Message::Pointer ChannelReceiveMessageQueue::pop(ChannelId channelId)
{
    auto& channelQueue = queue_.at(channelId);
    auto message(std::move(channelQueue.front()));
    channelQueue.pop();

    if(channelQueue.empty())
    {
        queue_.erase(channelId);
    }

    return message;
}

bool ChannelReceiveMessageQueue::empty(ChannelId channelId) const
{
    return queue_.count(channelId) == 0;
}

void ChannelReceiveMessageQueue::clear()
{
    queue_.clear();
}

}
}
}
