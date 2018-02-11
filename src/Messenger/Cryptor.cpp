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

#include <algorithm>
#include <functional>
#include <f1x/aasdk/Messenger/Cryptor.hpp>
#include <f1x/aasdk/Error/Error.hpp>

namespace f1x
{
namespace aasdk
{
namespace messenger
{

Cryptor::Cryptor(transport::ISSLWrapper::Pointer sslWrapper)
    : sslWrapper_(std::move(sslWrapper))
    , maxBufferSize_(1024 * 20)
    , certificate_(nullptr)
    , privateKey_(nullptr)
    , context_(nullptr)
    , ssl_(nullptr)
    , isActive_(false)
{

}

void Cryptor::init()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    certificate_ = sslWrapper_->readCertificate(cCertificate);

    if(certificate_ == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_READ_CERTIFICATE);
    }

    privateKey_ = sslWrapper_->readPrivateKey(cPrivateKey);

    if(privateKey_ == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_READ_PRIVATE_KEY);
    }

    auto method = sslWrapper_->getMethod();

    if(method == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_METHOD);
    }

    context_ = sslWrapper_->createContext(method);

    if(context_ == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_CONTEXT_CREATION);
    }

    if(!sslWrapper_->useCertificate(context_, certificate_))
    {
        throw error::Error(error::ErrorCode::SSL_USE_CERTIFICATE);
    }

    if(!sslWrapper_->usePrivateKey(context_, privateKey_))
    {
        throw error::Error(error::ErrorCode::SSL_USE_PRIVATE_KEY);
    }

    ssl_ = sslWrapper_->createInstance(context_);

    if(ssl_ == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_HANDLER_CREATION);
    }

    bIOs_ = sslWrapper_->createBIOs();

    if(bIOs_.first == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_READ_BIO_CREATION);
    }

    if(bIOs_.second == nullptr)
    {
        throw error::Error(error::ErrorCode::SSL_WRITE_BIO_CREATION);
    }

    sslWrapper_->setBIOs(ssl_, bIOs_, maxBufferSize_);

    sslWrapper_->setConnectState(ssl_);
}

void Cryptor::deinit()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if(ssl_ != nullptr)
    {
        sslWrapper_->free(ssl_);
        ssl_ = nullptr;
    }

    bIOs_ = std::make_pair(nullptr, nullptr);

    if(context_ != nullptr)
    {
        sslWrapper_->free(context_);
        context_ = nullptr;
    }

    if(certificate_ != nullptr)
    {
        sslWrapper_->free(certificate_);
        certificate_ = nullptr;
    }

    if(privateKey_ != nullptr)
    {
        sslWrapper_->free(privateKey_);
        privateKey_ = nullptr;
    }
}

bool Cryptor::doHandshake()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    auto result = sslWrapper_->doHandshake(ssl_);
    if(result == SSL_ERROR_WANT_READ)
    {
        return false;
    }
    else if(result == SSL_ERROR_NONE)
    {
        isActive_ = true;
        return true;
    }
    else
    {
        throw error::Error(error::ErrorCode::SSL_HANDSHAKE, result);
    }
}

size_t Cryptor::encrypt(common::Data& output, const common::DataConstBuffer& buffer)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    size_t totalWrittenBytes = 0;

    while(totalWrittenBytes < buffer.size)
    {
        const common::DataConstBuffer currentBuffer(buffer.cdata, buffer.size, totalWrittenBytes);
        const auto writeSize = sslWrapper_->sslWrite(ssl_, currentBuffer.cdata, currentBuffer.size);

        if(writeSize <= 0)
        {
            throw error::Error(error::ErrorCode::SSL_WRITE, sslWrapper_->getError(ssl_, writeSize));
        }

        totalWrittenBytes += writeSize;
    }

    return this->read(output);
}

size_t Cryptor::decrypt(common::Data& output, const common::DataConstBuffer& buffer)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    this->write(buffer);
    const size_t beginOffset = output.size();
    output.resize(beginOffset + 1);

    size_t availableBytes = 1;
    size_t totalReadSize = 0;

    while(availableBytes > 0)
    {
        const auto& currentBuffer = common::DataBuffer(output, totalReadSize + beginOffset);
        auto readSize = sslWrapper_->sslRead(ssl_, currentBuffer.data, currentBuffer.size);

        if(readSize <= 0)
        {
            throw error::Error(error::ErrorCode::SSL_READ, sslWrapper_->getError(ssl_, readSize));
        }

        totalReadSize += readSize;
        availableBytes = sslWrapper_->getAvailableBytes(ssl_);
        output.resize(output.size() + availableBytes);
    }

    return totalReadSize;
}

common::Data Cryptor::readHandshakeBuffer()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    common::Data output;
    this->read(output);
    return output;
}

void Cryptor::writeHandshakeBuffer(const common::DataConstBuffer& buffer)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    this->write(buffer);
}

size_t Cryptor::read(common::Data& output)
{
    const auto pendingSize = sslWrapper_->bioCtrlPending(bIOs_.second);

    size_t beginOffset = output.size();
    output.resize(beginOffset + pendingSize);
    size_t totalReadSize = 0;

    while(totalReadSize < pendingSize)
    {
        const auto& currentBuffer = common::DataBuffer(output, totalReadSize + beginOffset);
        const auto readSize = sslWrapper_->bioRead(bIOs_.second, currentBuffer.data, currentBuffer.size);

        if(readSize <= 0)
        {
            throw error::Error(error::ErrorCode::SSL_BIO_READ, sslWrapper_->getError(ssl_, readSize));
        }

        totalReadSize += readSize;
    }

    return totalReadSize;
}

void Cryptor::write(const common::DataConstBuffer& buffer)
{
    size_t totalWrittenBytes = 0;

    while(totalWrittenBytes < buffer.size)
    {
        const common::DataConstBuffer currentBuffer(buffer.cdata, buffer.size, totalWrittenBytes);
        const auto writeSize = sslWrapper_->bioWrite(bIOs_.first, currentBuffer.cdata, currentBuffer.size);

        if(writeSize <= 0)
        {
            throw error::Error(error::ErrorCode::SSL_BIO_WRITE, sslWrapper_->getError(ssl_, writeSize));
        }

        totalWrittenBytes += writeSize;
    }
}

bool Cryptor::isActive() const
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    return isActive_;
}

const std::string Cryptor::cCertificate = "-----BEGIN CERTIFICATE-----\n\
MIIDKjCCAhICARswDQYJKoZIhvcNAQELBQAwWzELMAkGA1UEBhMCVVMxEzARBgNV\n\
BAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDU1vdW50YWluIFZpZXcxHzAdBgNVBAoM\n\
Fkdvb2dsZSBBdXRvbW90aXZlIExpbmswJhcRMTQwNzA0MDAwMDAwLTA3MDAXETQ1\n\
MDQyOTE0MjgzOC0wNzAwMFMxCzAJBgNVBAYTAkpQMQ4wDAYDVQQIDAVUb2t5bzER\n\
MA8GA1UEBwwISGFjaGlvamkxFDASBgNVBAoMC0pWQyBLZW53b29kMQswCQYDVQQL\n\
DAIwMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM911mNnUfx+WJtx\n\
uk06GO7kXRW/gXUVNQBkbAFZmVdVNvLoEQNthi2X8WCOwX6n6oMPxU2MGJnvicP3\n\
6kBqfHhfQ2Fvqlf7YjjhgBHh0lqKShVPxIvdatBjVQ76aym5H3GpkigLGkmeyiVo\n\
VO8oc3cJ1bO96wFRmk7kJbYcEjQyakODPDu4QgWUTwp1Z8Dn41ARMG5OFh6otITL\n\
XBzj9REkUPkxfS03dBXGr5/LIqvSsnxib1hJ47xnYJXROUsBy3e6T+fYZEEzZa7y\n\
7tFioHIQ8G/TziPmvFzmQpaWMGiYfoIgX8WoR3GD1diYW+wBaZTW+4SFUZJmRKgq\n\
TbMNFkMCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAsGdH5VFn78WsBElMXaMziqFC\n\
zmilkvr85/QpGCIztI0FdF6xyMBJk/gYs2thwvF+tCCpXoO8mjgJuvJZlwr6fHzK\n\
Ox5hNUb06AeMtsUzUfFjSZXKrSR+XmclVd+Z6/ie33VhGePOPTKYmJ/PPfTT9wvT\n\
93qswcxhA+oX5yqLbU3uDPF1ZnJaEeD/YN45K/4eEA4/0SDXaWW14OScdS2LV0Bc\n\
YmsbkPVNYZn37FlY7e2Z4FUphh0A7yME2Eh/e57QxWrJ1wubdzGnX8mrABc67ADU\n\
U5r9tlTRqMs7FGOk6QS2Cxp4pqeVQsrPts4OEwyPUyb3LfFNo3+sP111D9zEow==\n\
-----END CERTIFICATE-----\n";

const std::string Cryptor::cPrivateKey = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAz3XWY2dR/H5Ym3G6TToY7uRdFb+BdRU1AGRsAVmZV1U28ugR\n\
A22GLZfxYI7Bfqfqgw/FTYwYme+Jw/fqQGp8eF9DYW+qV/tiOOGAEeHSWopKFU/E\n\
i91q0GNVDvprKbkfcamSKAsaSZ7KJWhU7yhzdwnVs73rAVGaTuQlthwSNDJqQ4M8\n\
O7hCBZRPCnVnwOfjUBEwbk4WHqi0hMtcHOP1ESRQ+TF9LTd0Fcavn8siq9KyfGJv\n\
WEnjvGdgldE5SwHLd7pP59hkQTNlrvLu0WKgchDwb9POI+a8XOZClpYwaJh+giBf\n\
xahHcYPV2Jhb7AFplNb7hIVRkmZEqCpNsw0WQwIDAQABAoIBAB2u7ZLheKCY71Km\n\
bhKYqnKb6BmxgfNfqmq4858p07/kKG2O+Mg1xooFgHrhUhwuKGbCPee/kNGNrXeF\n\
pFW9JrwOXVS2pnfaNw6ObUWhuvhLaxgrhqLAdoUEgWoYOHcKzs3zhj8Gf6di+edq\n\
SyTA8+xnUtVZ6iMRKvP4vtCUqaIgBnXdmQbGINP+/4Qhb5R7XzMt/xPe6uMyAIyC\n\
y5Fm9HnvekaepaeFEf3bh4NV1iN/R8px6cFc6ELYxIZc/4Xbm91WGqSdB0iSriaZ\n\
TjgrmaFjSO40tkCaxI9N6DGzJpmpnMn07ifhl2VjnGOYwtyuh6MKEnyLqTrTg9x0\n\
i3mMwskCgYEA9IyljPRerXxHUAJt+cKOayuXyNt80q9PIcGbyRNvn7qIY6tr5ut+\n\
ZbaFgfgHdSJ/4nICRq02HpeDJ8oj9BmhTAhcX6c1irH5ICjRlt40qbPwemIcpybt\n\
mb+DoNYbI8O4dUNGH9IPfGK8dRpOok2m+ftfk94GmykWbZF5CnOKIp8CgYEA2Syc\n\
5xlKB5Qk2ZkwXIzxbzozSfunHhWWdg4lAbyInwa6Y5GB35UNdNWI8TAKZsN2fKvX\n\
RFgCjbPreUbREJaM3oZ92o5X4nFxgjvAE1tyRqcPVbdKbYZgtcqqJX06sW/g3r/3\n\
RH0XPj2SgJIHew9sMzjGWDViMHXLmntI8rVA7d0CgYBOr36JFwvrqERN0ypNpbMr\n\
epBRGYZVSAEfLGuSzEUrUNqXr019tKIr2gmlIwhLQTmCxApFcXArcbbKs7jTzvde\n\
PoZyZJvOr6soFNozP/YT8Ijc5/quMdFbmgqhUqLS5CPS3z2N+YnwDNj0mO1aPcAP\n\
STmcm2DmxdaolJksqrZ0owKBgQCD0KJDWoQmaXKcaHCEHEAGhMrQot/iULQMX7Vy\n\
gl5iN5E2EgFEFZIfUeRWkBQgH49xSFPWdZzHKWdJKwSGDvrdrcABwdfx520/4MhK\n\
d3y7CXczTZbtN1zHuoTfUE0pmYBhcx7AATT0YCblxrynosrHpDQvIefBBh5YW3AB\n\
cKZCOQKBgEM/ixzI/OVSZ0Py2g+XV8+uGQyC5XjQ6cxkVTX3Gs0ZXbemgUOnX8co\n\
eCXS4VrhEf4/HYMWP7GB5MFUOEVtlLiLM05ruUL7CrphdfgayDXVcTPfk75lLhmu\n\
KAwp3tIHPoJOQiKNQ3/qks5km/9dujUGU2ARiU3qmxLMdgegFz8e\n\
-----END RSA PRIVATE KEY-----\n";

}
}
}
