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

#include <boost/algorithm/hex.hpp>
#include <f1x/aasdk/Common/Data.hpp>
#include <f1x/aasdk/Common/Log.hpp>

namespace f1x
{
namespace aasdk
{
namespace common
{

DataBuffer::DataBuffer()
    : data(nullptr)
    , size(0)
{

}

DataBuffer::DataBuffer(Data::value_type* _data, Data::size_type _size, Data::size_type offset)
{
    if(offset > _size || _data == nullptr || _size == 0)
    {
        data = nullptr;
        size = 0;
    }
    else if(offset <= _size)
    {
        data = _data + offset;
        size = _size - offset;
    }
}

DataBuffer::DataBuffer(void* _data, Data::size_type _size, Data::size_type offset)
    : DataBuffer(reinterpret_cast<Data::value_type*>(_data), _size, offset)
{

}

DataBuffer::DataBuffer(Data& _data, Data::size_type offset)
    : DataBuffer(_data.empty() ? nullptr : &_data[0], _data.size(), offset > _data.size() ? 0 : offset)
{

}

bool DataBuffer::operator==(const std::nullptr_t&) const
{
    return data == nullptr || size == 0;
}

DataConstBuffer::DataConstBuffer()
    : cdata(nullptr)
    , size(0)
{

}

DataConstBuffer::DataConstBuffer(const DataBuffer& other)
    : cdata(other.data)
    , size(other.size)
{
}

DataConstBuffer::DataConstBuffer(const Data::value_type* _data, Data::size_type _size, Data::size_type offset)
{
    if(offset > _size || _data == nullptr || _size == 0)
    {
        cdata = nullptr;
        size = 0;
    }
    else if(offset <= _size)
    {
        cdata = _data + offset;
        size = _size - offset;
    }
}

DataConstBuffer::DataConstBuffer(const void* _data, Data::size_type _size, Data::size_type offset)
    : DataConstBuffer(reinterpret_cast<const Data::value_type*>(_data), _size, offset)
{

}

DataConstBuffer::DataConstBuffer(const Data& _data, Data::size_type offset)
    : DataConstBuffer(_data.empty() ? nullptr : &_data[0], _data.size(), offset > _data.size() ? 0 : offset)
{

}

bool DataConstBuffer::operator==(const std::nullptr_t&) const
{
    return cdata == nullptr || size == 0;
}

common::Data createData(const DataConstBuffer& buffer)
{
    common::Data data;
    copy(data, buffer);
    return data;
}

std::string dump(const Data& data)
{
    return dump(DataConstBuffer(data));
}

std::string dump(const DataConstBuffer& buffer)
{
    if(buffer.size == 0)
    {
        return "[0] null";
    }
    else
    {
        std::string hexDump = "[" + std::to_string(buffer.size) + "] ";
        //boost::algorithm::hex(bufferBegin(buffer), bufferEnd(buffer), back_inserter(hexDump));
        return hexDump;
    }
}

}
}
}
