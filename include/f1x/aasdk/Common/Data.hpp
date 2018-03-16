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

#include <vector>
#include <string>
#include <cstddef>
#include <stdint.h>

namespace f1x
{
namespace aasdk
{
namespace common
{

typedef std::vector<uint8_t> Data;

static constexpr size_t cStaticDataSize = 30 * 1024 * 1024;

struct DataBuffer
{
    DataBuffer();
    DataBuffer(Data::value_type* _data, Data::size_type _size, Data::size_type offset = 0);
    DataBuffer(void* _data, Data::size_type _size, Data::size_type offset = 0);
    explicit DataBuffer(Data& _data, Data::size_type offset = 0);
    bool operator==(const std::nullptr_t&) const;
    bool operator==(const DataBuffer& buffer) const;

    Data::value_type* data;
    Data::size_type size;
};

struct DataConstBuffer
{
    DataConstBuffer();
    explicit DataConstBuffer(const DataBuffer& other);
    DataConstBuffer(const Data::value_type* _data, Data::size_type _size, Data::size_type offset = 0);
    DataConstBuffer(const void* _data, Data::size_type _size, Data::size_type offset = 0);
    explicit DataConstBuffer(const Data& _data, Data::size_type offset = 0);
    bool operator==(const std::nullptr_t&) const;
    bool operator==(const DataConstBuffer& buffer) const;

    const Data::value_type* cdata;
    Data::size_type size;
};

template<typename DataType>
void copy(DataType& data, const DataBuffer& buffer)
{
    size_t offset = data.size();
    data.resize(data.size() + buffer.size);
    memcpy(&data[offset], buffer.data, buffer.size);
}

template<typename DataType>
void copy(DataType& data, const DataConstBuffer& buffer)
{
    size_t offset = data.size();
    data.resize(data.size() + buffer.size);
    memcpy(&data[offset], buffer.cdata, buffer.size);
}

common::Data createData(const DataConstBuffer& buffer);

std::string dump(const Data& data);
std::string dump(const DataConstBuffer& buffer);

}
}
}
