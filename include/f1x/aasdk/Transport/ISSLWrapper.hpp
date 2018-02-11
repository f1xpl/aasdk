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
#include <openssl/ssl.h>

namespace f1x
{
namespace aasdk
{
namespace transport
{

class ISSLWrapper
{
public:
    typedef std::pair<BIO*, BIO*> BIOs;
    typedef std::shared_ptr<ISSLWrapper> Pointer;

    ISSLWrapper() = default;
    virtual ~ISSLWrapper() = default;

    virtual X509* readCertificate(const std::string& certificate) = 0;
    virtual EVP_PKEY* readPrivateKey(const std::string& privateKey) = 0;
    virtual const SSL_METHOD* getMethod() = 0;
    virtual SSL_CTX* createContext(const SSL_METHOD* method) = 0;
    virtual bool useCertificate(SSL_CTX* context, X509* certificate) = 0;
    virtual bool usePrivateKey(SSL_CTX* context, EVP_PKEY* privateKey) = 0;
    virtual SSL* createInstance(SSL_CTX* context) = 0;
    virtual bool checkPrivateKey(SSL* ssl) = 0;
    virtual std::pair<BIO*, BIO*> createBIOs() = 0;
    virtual void setBIOs(SSL* ssl, const BIOs& bIOs, size_t maxBufferSize) = 0;
    virtual void setConnectState(SSL* ssl) = 0;
    virtual int doHandshake(SSL* ssl) = 0;
    virtual void free(SSL* ssl) = 0;
    virtual void free(SSL_CTX* context) = 0;
    virtual void free(BIO* bio) = 0;
    virtual void free(X509* certificate) = 0;
    virtual void free(EVP_PKEY* privateKey) = 0;

    virtual size_t bioCtrlPending(BIO* b) = 0;
    virtual int bioRead(BIO *b, void *data, int len) = 0;
    virtual int bioWrite(BIO *b, const void *data, int len) = 0;

    virtual int getAvailableBytes(const SSL* ssl) = 0;
    virtual int sslRead(SSL *ssl, void *buf, int num) = 0;
    virtual int sslWrite(SSL *ssl, const void *buf, int num) = 0;
    virtual int getError(SSL* ssl, int returnCode) = 0;
};

}
}
}
