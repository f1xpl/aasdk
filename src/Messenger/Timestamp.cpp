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
#include <f1x/aasdk/Messenger/Timestamp.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

Timestamp::Timestamp(ValueType stamp)
    : stamp_(stamp)
{

}

Timestamp::Timestamp(const common::DataConstBuffer& buffer)
{
    const ValueType& timestampBig = reinterpret_cast<const ValueType&>(buffer.cdata[0]);
    stamp_ = boost::endian::big_to_native(timestampBig);
}

common::Data Timestamp::getData() const
{
    const ValueType timestampBig = boost::endian::native_to_big(stamp_);
    const common::DataConstBuffer timestampBuffer(&timestampBig, sizeof(timestampBig));
    return common::createData(timestampBuffer);
}

Timestamp::ValueType Timestamp::getValue() const
{
    return stamp_;
}

}
}
}
