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

#include <f1x/aasdk/Messenger/ChannelId.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

std::string channelIdToString(ChannelId channelId)
{
    switch(channelId)
    {
    case ChannelId::CONTROL:
        return "CONTROL";
    case ChannelId::INPUT:
        return "INPUT";
    case ChannelId::SENSOR:
        return "SENSOR";
    case ChannelId::VIDEO:
        return "VIDEO";
    case ChannelId::MEDIA_AUDIO:
        return "MEDIA_AUDIO";
    case ChannelId::SPEECH_AUDIO:
        return "SPEECH_AUDIO";
    case ChannelId::SYSTEM_AUDIO:
        return "SYSTEM_AUDIO";
    case ChannelId::AV_INPUT:
        return "AV_INPUT";
    case ChannelId::BLUETOOTH:
        return "BLUETOOTH";
    case ChannelId::NONE:
        return "NONE";
    default:
        return "(null)";
    }
}

}
}
}
