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

#include <mutex>
#include <f1x/aasdk/Transport/ISSLWrapper.hpp>
#include <f1x/aasdk/Messenger/ICryptor.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

class Cryptor: public ICryptor
{
public:
    Cryptor(transport::ISSLWrapper::Pointer sslWrapper);

    void init() override;
    void deinit() override;
    bool doHandshake() override;
    size_t encrypt(common::Data& output, const common::DataConstBuffer& buffer) override;
    size_t decrypt(common::Data& output, const common::DataConstBuffer& buffer) override;

    common::Data readHandshakeBuffer() override;
    void writeHandshakeBuffer(const common::DataConstBuffer& buffer) override;
    bool isActive() const override;

private:
    size_t read(common::Data& output);
    void write(const common::DataConstBuffer& buffer);

    transport::ISSLWrapper::Pointer sslWrapper_;
    size_t maxBufferSize_;
    X509* certificate_;
    EVP_PKEY* privateKey_;
    SSL_CTX* context_;
    SSL* ssl_;
    transport::ISSLWrapper::BIOs bIOs_;
    bool isActive_;

    const static std::string cCertificate;
    const static std::string cPrivateKey;
    mutable std::mutex mutex_;
};

}
}
}
