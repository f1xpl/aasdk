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

#include <memory>
#include <f1x/aasdk/Common/Data.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class ICryptor
{
public:
    typedef std::shared_ptr<ICryptor> Pointer;

    ICryptor() = default;
    virtual ~ICryptor() = default;

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual bool doHandshake() = 0;
    virtual size_t encrypt(common::Data& output, const common::DataConstBuffer& buffer) = 0;
    virtual size_t decrypt(common::Data& output, const common::DataConstBuffer& buffer) = 0;
    virtual common::Data readHandshakeBuffer() = 0;
    virtual void writeHandshakeBuffer(const common::DataConstBuffer& buffer) = 0;
    virtual bool isActive() const = 0;
};

}
}
}
