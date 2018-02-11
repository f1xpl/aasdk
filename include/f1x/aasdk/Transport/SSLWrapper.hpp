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

#include <f1x/aasdk/Transport/ISSLWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

class SSLWrapper: public ISSLWrapper
{
public:
    SSLWrapper();
    ~SSLWrapper() override;

    X509* readCertificate(const std::string& certificate) override;
    EVP_PKEY* readPrivateKey(const std::string& privateKey) override;
    const SSL_METHOD* getMethod() override;
    SSL_CTX* createContext(const SSL_METHOD* method) override;
    bool useCertificate(SSL_CTX* context, X509* certificate) override;
    bool usePrivateKey(SSL_CTX* context, EVP_PKEY* privateKey) override;
    SSL* createInstance(SSL_CTX* context) override;
    bool checkPrivateKey(SSL* ssl) override;
    BIOs createBIOs() override;
    void setBIOs(SSL* ssl, const BIOs& bIOs, size_t maxBufferSize) override;
    void setConnectState(SSL* ssl) override;
    int doHandshake(SSL* ssl) override;
    int getError(SSL* ssl, int returnCode) override;

    void free(SSL* ssl) override;
    void free(SSL_CTX* context) override;
    void free(BIO* bio) override;
    void free(X509* certificate) override;
    void free(EVP_PKEY* privateKey) override;

    size_t bioCtrlPending(BIO* b) override;
    int bioRead(BIO *b, void *data, int len) override;
    int bioWrite(BIO *b, const void *data, int len) override;
    int getAvailableBytes(const SSL* ssl) override;
    int sslRead(SSL *ssl, void *buf, int num) override;
    int sslWrite(SSL *ssl, const void *buf, int num) override;
};

}
}
}
