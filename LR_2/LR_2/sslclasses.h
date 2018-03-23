#ifndef SSLCLASSES_H
#define SSLCLASSES_H

#include <string>
#include <memory>
#include <openssl/ssl.h>

#include "socketapi.h"
#include "socket.h"

/* ========================================= *
 *             Класс SSLException;           *
 * Класс-исключение для сообщения об ошибке. *
 * Параметры:                                *
 * * msg - Сообщение об ошибке.              *
 * ========================================= */
class SSLException : public std::exception
{
    std::string _msg;

public:
    virtual const char* what() const throw();

    SSLException();
    SSLException(const std::string& msg);
};



/* ========================================== *
 *             Класс SSLInitializer;          *
 * Класс, отвечающий за инициализацию OpenSSL *
 * ========================================== */
class SSLInitializer
{
public:
    static void initOpenSSL();
};


/* ======================================================== *
 *                     Класс SSLContext;                    *
 * Предназначен для создание и инициализации контекста SSL. *
 * Параметры:                                               *
 * - type                - Тип контекста(Клиент или сервер) *
 * - sertificateFilePath - Путь к файлу-сертификату.        *
 * - privateKeyFilePath  - Путь к файлу-закрытому ключу.    *
 * ======================================================== */
class SSLContext
{
public:
    enum SSLContextType
    {
        SSL_Client,
        SSL_Server
    };

private:
    std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)> _sslContext;

public:
    SSLContext(SSLContextType type);

    SSLContext(SSLContextType type, std::string sertificateFilePath, std::string privateKeyFilePath);

    inline SSL_CTX* getSSLCTX();

    inline operator SSL_CTX* () { return _sslContext.get(); }


private:
    void m_generatePrivateKeyAndCertificate();
};



/* ============================================================= *
 *                       Класс SSLConnect;                       *
 * Предназначен для создание и инициализации контекста SSL.      *
 * Параметры:                                                    *
 * - sslContex - SSL контекст типа SSLContext.                   *
 * - sock - Сокет, через который будет происходить обмен данными *
 * ============================================================= */
class SSLConnect
{
private:
    bool _hasSertificate = false;
    std::unique_ptr<SSL, decltype(&::SSL_free)> _ssl;

    std::string _subject_name;
    std::string _issuer_name;

public:
    SSLConnect(SSLContext& sslContext, Socket sock);
    virtual ~SSLConnect() {}
    void accept();
    void connect();

    bool haveSertificate();
    const std::string& getSubjectName();
    const std::string& getIssuerName();

    SSL* getSSL();

    inline operator SSL* () { return _ssl.get(); }


    int send(const std::string& msg);

    std::pair<int, std::string> revc();


private:
    void getCertificate();
};



/* ============================================================= *
 *                         Оператор >>                           *
 * Реализует более удобный способ чтения данных из буфера сокета *
 * Пример:                                                       *
 *      SSLConnect connect(sslContext, connectSocket);           *
 *      std::string messageFromClient;                           *
 *      int bytes = connect >> messageFromClient;                *
 *      if (bytes > 0)                                           *
 *          std::cout << messageFromClient << std::endl;         *
 * ============================================================= */
int operator >> (SSLConnect& connect, std::string& msg);

/* ========================================================== *
 *                        Оператор <<                         *
 * Реализует более удобный способ отправки данных через сокет *
 * Пример:                                                    *
 *      SSLConnect connect(sslContext, connectSocket);        *
 *      std::string messageToClient = "Hello, client!";       *
 *      int bytes = connect << messageToClient;               *
 *      if (bytes > 0)                                        *
 *          std::cout << "Message was sended!" << std::endl;  *
 * ========================================================== */
int operator << (SSLConnect& connect, const std::string& msg);

#endif // SSLCLASSES_H
