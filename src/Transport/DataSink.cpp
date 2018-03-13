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

#include <cstring>
#include <f1x/aasdk/Transport/DataSink.hpp>
#include <f1x/aasdk/Error/Error.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

DataSink::DataSink()
    : data_(common::cStaticDataSize)
{
}

common::DataBuffer DataSink::fill()
{
    const auto offset = data_.size();
    data_.resize(data_.size() + cChunkSize);

    return common::DataBuffer(&data_[offset], cChunkSize);
}

void DataSink::commit(common::Data::size_type size)
{
    if(size > cChunkSize)
    {
        throw error::Error(error::ErrorCode::DATA_SINK_COMMIT_OVERFLOW);
    }

    data_.resize(data_.size() - (cChunkSize - size));
}

common::Data::size_type DataSink::getAvailableSize()
{
    return data_.size();
}

common::Data DataSink::consume(common::Data::size_type size)
{
    if(size > data_.size())
    {
        throw error::Error(error::ErrorCode::DATA_SINK_CONSUME_UNDERFLOW);
    }

    common::Data data;
    common::copy(data, common::DataConstBuffer(&data_[0], size));
    data_.erase(data_.begin(), data_.begin() + size);

    return data;
}

}
}
}
