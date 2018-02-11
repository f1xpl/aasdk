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

#include <iomanip>
#include <boost/endian/conversion.hpp>
#include <f1x/aasdk/Messenger/MessageId.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

MessageId::MessageId(uint16_t id):
    id_(id)
{

}
MessageId::MessageId(const common::Data& data)
{
    id_ = boost::endian::big_to_native(reinterpret_cast<const uint16_t&>(data[0]));
}

uint16_t MessageId::getId() const
{
    return id_;
}

common::Data MessageId::getData() const
{
    const MessageId messageIdBig = boost::endian::native_to_big(id_);
    const common::DataConstBuffer messageIdBigBuffer(&messageIdBig, sizeof(messageIdBig));
    return common::createData(messageIdBigBuffer);
}

bool MessageId::operator>(uint16_t id) const
{
    return id > id_;
}

bool MessageId::operator<(uint16_t id) const
{
    return id < id_;
}

bool MessageId::operator==(uint16_t id) const
{
    return id == id_;
}

MessageId& MessageId::operator=(uint16_t id)
{
    id_ = id;
    return *this;
}

bool MessageId::operator>=(uint16_t id) const
{
    return id >= id_;
}

bool MessageId::operator<=(uint16_t id) const
{
    return id <= id_;
}

std::ostream& operator<<(std::ostream& stream, const f1x::aasdk::messenger::MessageId& messageId)
{
    stream << "0x" << std::hex << messageId.getId() << std::dec;
    return stream;
}

}
}
}
