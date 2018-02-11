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

#include <gmock/gmock.h>
#include <f1x/aasdk/Messenger/ICryptor.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{
namespace ut
{

class CryptorMock: public ICryptor
{
public:
    MOCK_METHOD0(init, void());
    MOCK_METHOD0(deinit, void());
    MOCK_METHOD0(doHandshake, bool());
    MOCK_METHOD2(encrypt, size_t(common::Data& output, const common::DataConstBuffer& buffer));
    MOCK_METHOD2(decrypt, size_t(common::Data& output, const common::DataConstBuffer& buffer));
    MOCK_METHOD0(readHandshakeBuffer, common::Data());
    MOCK_METHOD1(writeHandshakeBuffer, void(const common::DataConstBuffer& buffer));
    MOCK_CONST_METHOD0(isActive, bool());
};

}
}
}
}
