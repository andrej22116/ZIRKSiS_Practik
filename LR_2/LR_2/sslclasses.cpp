#include "sslclasses.h"


/* ---------------------------------------------------------------------------------------------------------------------------------------------- *
 * SSLException SSLException SSLException SSLException SSLException SSLException SSLException SSLException SSLException SSLException SSLException *
 * ---------------------------------------------------------------------------------------------------------------------------------------------- */
const char* SSLException::what() const throw()
{
    return _msg.c_str();
}

SSLException::SSLException() : _msg("OpenSSL error!") {}
SSLException::SSLException(const std::string& msg) : _msg(msg) {}



/* -------------------------------------------------------------------------------------------------------------------------------------- *
 * SSLInitializer SSLInitializer SSLInitializer SSLInitializer SSLInitializer SSLInitializer SSLInitializer SSLInitializer SSLInitializer *
 * -------------------------------------------------------------------------------------------------------------------------------------- */
void SSLInitializer::initOpenSSL()
{
    SSL_load_error_strings();

    OpenSSL_add_all_algorithms();
    SSL_library_init();
}





/* ---------------------------------------------------------------------------------------------------------------------------------------------- *
 * SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext SSLContext *
 * ---------------------------------------------------------------------------------------------------------------------------------------------- */
SSLContext::SSLContext(SSLContextType type) :
    _sslContext((type == SSL_Server) ? SSL_CTX_new(SSLv23_server_method())
                                     : SSL_CTX_new(SSLv23_client_method()), ::SSL_CTX_free)

{
    if (!_sslContext.get())
    {
        throw SSLException();
    }
    if (type == SSL_Server)
    {
        m_generatePrivateKeyAndCertificate();
    }
}

SSLContext::SSLContext(SSLContextType type, std::string sertificateFilePath, std::string privateKeyFilePath) :
    _sslContext((type == SSL_Server) ? SSL_CTX_new(SSLv23_server_method())
                                     : SSL_CTX_new(SSLv23_client_method()), ::SSL_CTX_free)
{
    if (!_sslContext.get())
    {
        throw SSLException();
    }

    if (type == SSL_Server)
    {
        if (SSL_CTX_use_certificate_file(_sslContext.get(), sertificateFilePath.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            throw SSLException("Unable to install certificate!");
        }
        if (SSL_CTX_use_PrivateKey_file(_sslContext.get(), privateKeyFilePath.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            throw SSLException("Unable to install private key!");
        }

        if (!SSL_CTX_check_private_key(_sslContext.get()))
        {
            throw SSLException("Private key does not match the certificate public key");
        }
    }
}

SSL_CTX* SSLContext::getSSLCTX() { return _sslContext.get(); }

void SSLContext::m_generatePrivateKeyAndCertificate()
{
    EVP_PKEY* privateKey = nullptr;
    EVP_PKEY_CTX *privateKeyContext = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY_keygen_init(privateKeyContext);
    EVP_PKEY_CTX_set_rsa_keygen_bits(privateKeyContext, 2048);
    EVP_PKEY_keygen(privateKeyContext, &privateKey);

    if (SSL_CTX_use_PrivateKey(_sslContext.get(), privateKey) <= 0)
    {
        EVP_PKEY_free(privateKey);
        EVP_PKEY_CTX_free(privateKeyContext);
        throw SSLException("Error in generate private key!");
    }

    X509* x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 0);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), (long)60*60*24*365);
    X509_set_pubkey(x509, privateKey);

    X509_NAME* name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char*)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (const unsigned char*)"Polotsk State University", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char*)"Super student!", -1, -1, 0);
    X509_set_issuer_name(x509, name);
    X509_sign(x509, privateKey, EVP_md5());


    if (SSL_CTX_use_certificate(_sslContext.get(), x509) <= 0)
    {
        X509_NAME_free(name);
        EVP_PKEY_free(privateKey);
        EVP_PKEY_CTX_free(privateKeyContext);
        X509_free(x509);
        throw SSLException("Error in generate certificate!");
    }
}





/* ---------------------------------------------------------------------------------------------------------------------------------------------- *
 * SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect SSLConnect *
 * ---------------------------------------------------------------------------------------------------------------------------------------------- */
SSLConnect::SSLConnect(SSLContext& sslContext, Socket sock) :
    _ssl(SSL_new(sslContext), ::SSL_free)
{
    if (!_ssl)
    {
        throw SSLException("Create SSL connect error!");
    }

    SSL_set_fd(_ssl.get(), sock);
}
void SSLConnect::accept()
{
    if (SSL_accept(_ssl.get()) == -1)
    {
        throw SSLException("SSL accept error!");
    }
    getCertificate();
}
void SSLConnect::connect()
{
    if (SSL_connect(_ssl.get()) == -1)
    {
        throw SSLException("SSL connect error!");
    }
    getCertificate();
}

bool SSLConnect::haveSertificate() { return _hasSertificate; }
const std::string& SSLConnect::getSubjectName() { return _subject_name; }
const std::string& SSLConnect::getIssuerName() { return _issuer_name; }

SSL* SSLConnect::getSSL() { return _ssl.get(); }

int SSLConnect::send(const std::string& msg)
{
    int dataSize = ::SSL_write(_ssl.get(), msg.c_str(), sizeof(char) * msg.length());
    if (dataSize == -1)
    {
        throw SSLException("Connect error!");
    }

    return dataSize;
}

std::pair<int, std::string> SSLConnect::revc()
{
    const int bufSize = 64;
    std::pair<int, std::string> res = {0, ""};
    while (true)
    {
        int dataSize = 0;
        char buf[bufSize + 1] = { 0 };

        dataSize = SSL_read(_ssl.get(), buf, sizeof(char) * bufSize);
        if (dataSize == -1)
        {
            throw SSLException("Connect error!");
        }

        res.first += dataSize;
        res.second += buf;
        if (dataSize < bufSize)
        {
            return res;
        }
    }
}

void SSLConnect::getCertificate()
{
    X509* sertificateInfo = SSL_get_peer_certificate(_ssl.get());
    if (sertificateInfo)
    {
        _hasSertificate = true;

        char* subjName = X509_NAME_oneline(X509_get_subject_name(sertificateInfo), 0, 0);
        if (subjName)
        {
            _subject_name = subjName;
        }

        char* issureName = X509_NAME_oneline(X509_get_issuer_name(sertificateInfo), 0, 0);
        if (issureName)
        {
            _issuer_name = issureName;
        }

        X509_free(sertificateInfo);
    }
}


int operator >> (SSLConnect& connect, std::string& msg)
{
    auto res = connect.revc();
    msg = res.second;
    return res.first;
}

int operator << (SSLConnect& connect, const std::string& msg)
{
    return connect.send(msg);
}
