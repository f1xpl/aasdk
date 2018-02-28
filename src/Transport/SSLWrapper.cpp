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

#include <string>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <f1x/aasdk/Transport/SSLWrapper.hpp>

namespace f1x
{
namespace aasdk
{
namespace transport
{

SSLWrapper::SSLWrapper()
{
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
}

SSLWrapper::~SSLWrapper()
{
    FIPS_mode_set(0);
    ENGINE_cleanup();
    CONF_modules_unload(1);
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    ERR_remove_state(0);
#endif
    ERR_free_strings();
}

X509* SSLWrapper::readCertificate(const std::string& certificate)
{
    auto bio = BIO_new_mem_buf(certificate.c_str(), certificate.size());
    X509* x509Certificate = PEM_read_bio_X509_AUX(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    return x509Certificate;
}

EVP_PKEY* SSLWrapper::readPrivateKey(const std::string& privateKey)
{
    auto bio = BIO_new_mem_buf(privateKey.c_str(), privateKey.size());
    auto result = PEM_read_bio_PrivateKey (bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    return result;
}

const SSL_METHOD* SSLWrapper::getMethod()
{
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    return TLSv1_2_client_method();
#else
    return TLS_client_method();
#endif
}

SSL_CTX* SSLWrapper::createContext(const SSL_METHOD* method)
{
    return SSL_CTX_new(method);
}

bool SSLWrapper::useCertificate(SSL_CTX* context, X509* certificate)
{
    return SSL_CTX_use_certificate(context, certificate) == 1;
}

bool SSLWrapper::usePrivateKey(SSL_CTX* context, EVP_PKEY* privateKey)
{
    return SSL_CTX_use_PrivateKey(context, privateKey) == 1;
}

SSL* SSLWrapper::createInstance(SSL_CTX* context)
{
    return SSL_new(context);
}

bool SSLWrapper::checkPrivateKey(SSL* ssl)
{
    return SSL_check_private_key(ssl) == 1;
}

std::pair<BIO*, BIO*> SSLWrapper::createBIOs()
{
    auto readBIO = BIO_new(BIO_s_mem());
    auto writeBIO = BIO_new(BIO_s_mem());
    return std::make_pair(readBIO, writeBIO);
}

void SSLWrapper::setBIOs(SSL* ssl, const BIOs& bIOs, size_t maxBufferSize)
{
    SSL_set_bio(ssl, bIOs.first, bIOs.second);
    BIO_set_write_buf_size(bIOs.first, maxBufferSize);
    BIO_set_write_buf_size(bIOs.second, maxBufferSize);
}

void SSLWrapper::setConnectState(SSL* ssl)
{
    SSL_set_connect_state(ssl);
    SSL_set_verify(ssl, SSL_VERIFY_NONE, nullptr);
}

int SSLWrapper::doHandshake(SSL* ssl)
{
    auto result = SSL_do_handshake(ssl);
    auto errorCode = SSL_get_error(ssl, result);

    return errorCode;
}

void SSLWrapper::free(SSL* ssl)
{
    SSL_free(ssl);
}

void SSLWrapper::free(SSL_CTX* context)
{
    SSL_CTX_free(context);
}

void SSLWrapper::free(BIO* bio)
{
    BIO_free(bio);
}

void SSLWrapper::free(X509* certificate)
{
    X509_free(certificate);
}

void SSLWrapper::free(EVP_PKEY* privateKey)
{
    EVP_PKEY_free(privateKey);
}

size_t SSLWrapper::bioCtrlPending(BIO* b)
{
    return BIO_ctrl_pending(b);
}

int SSLWrapper::bioRead(BIO *b, void *data, int len)
{
    return BIO_read(b, data, len);
}

int SSLWrapper::bioWrite(BIO *b, const void *data, int len)
{
    return BIO_write(b, data, len);
}

int SSLWrapper::getAvailableBytes(const SSL* ssl)
{
    return SSL_pending(ssl);
}

int SSLWrapper::sslRead(SSL *ssl, void *buf, int num)
{
    return SSL_read(ssl, buf, num);
}

int SSLWrapper::sslWrite(SSL *ssl, const void *buf, int num)
{
    return SSL_write(ssl, buf, num);
}

int SSLWrapper::getError(SSL* ssl, int returnCode)
{
    return SSL_get_error(ssl, returnCode);
}

}
}
}
