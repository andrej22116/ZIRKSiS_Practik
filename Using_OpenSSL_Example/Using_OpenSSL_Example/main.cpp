#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cstring>

#include <openssl/ssl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

/* ========================================================================================================================= *
 *
 * Для моих лапочек и не только :3 *
 * Этот говнокод пишу для тех, у кого есть проблемы с написанием своих SSL клиента и сервера
 * (В основном это те люди, которые программируют на других ЯП и не желающие курить этот дурацкий C++)
 *
 * Немного теории для понимания происходящего:
 * 1) Умные указатели (unique_ptr, shared_ptr) - обёртки над обычными указателями. Цель - освобождать ресурсы,
 * на которые ссылается указатель, после того, как объект(или все объекты, в случае с shared_ptr) был уничтожен.
 * 1.1) unique_ptr - умный указатель, который нужен для единоличного хранения адреса объекта. Если выполнить
 * операцию копирования, то данные из одно умного указателя перейдут в другой, первый забудет всё, что он знал до этого.
 *      Пример:
 *          unique_ptr<int> first(new int(228)); <- Создали умный указатель и передали ему данные.
 *          unique_ptr<int> second = first; <- Перенесли данные из first в second (first больше не ссылается на число 228)
 * 1.2) Для получения самого указателя вызывается метод get()
 * 1.3) Умный указатель можно разименовать так же, как и обычный указатель (получив доступ к содержимомум)
 * Так же этот тип позволяет передавать кастомные освободители памяти, если таковые имеются.
 *
 *
 * Для упрощения я ввёл некоторые ассоциации с типами, что бы было удобнее использовать.
 * Особенно это касается умных указателей:
 *      Типичная строка:> using TypeName_ptr = unique_ptr<TypeName, TypeName_free>;
 *      Где TypeName_ptr - новый тип, за которым будет скрываться умный указатель.
 *          TypeName - Тип, на который будет ссылаться указатель
 *          TypeName_free - Функция, которая будет использована для освобождения ресурсов.
 *      Для создания объектов я ввёл процедуры, которые возвращают готовые объекты (Это должно упростить
 *          понимание происходящего.
 *
 *                                                      !!!ВАЖНО!!!
 * Я не навязываю вам использование умных указателей и прочего. Вы в праве использовать то, что вы считаете нужным.
 * Опытные C++ рекомендуют использовать указатели напрямую только в случае крайней необходимости.
 *
 * ========================================================================================================================= */

// Для компактности
using u_short = unsigned short;
using u_int = unsigned int;

// Умный указатель на объект EVP_PKEY
using EVP_PKEY_ptr = unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
// Конструктор объектов типа EVP_PKEY_ptr
inline EVP_PKEY_ptr get_EVP_PKEY_ptr() { return EVP_PKEY_ptr(EVP_PKEY_new(), ::EVP_PKEY_free); }


// Умный указатель на объект SSL_CTX
using SSL_CTX_ptr = unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)>;
/* ========================================================== *
 * Конструктор объектов типа SSL_CTX_ptr
 * Можете отправить сюда SSLv23_server_method() - для сервера
 * Или SSLv23_client_method() - для клиента :3
 * ========================================================== */
inline SSL_CTX_ptr get_SSL_CTX_ptr(const SSL_METHOD* method) { return SSL_CTX_ptr(SSL_CTX_new(method), ::SSL_CTX_free); }


// Умный указатель на объект SSL
using SSL_ptr = std::unique_ptr<SSL, decltype(&::SSL_free)>;
/* ================================= *
 * Конструктор объектов типа SSL_ptr
 * Принимает:
 *      объект SSL_CTX_ptr
 *      сокет
 * ================================= */
inline SSL_ptr get_SSL_ptr(SSL_CTX_ptr& ssl_ctx_ptr, int socket)
{
    // Создаём объект SSL_ptr
    SSL_ptr ssl_ptr(SSL_new(ssl_ctx_ptr.get()), ::SSL_free);
    // Связываем его с сокетом
    SSL_set_fd(ssl_ptr.get(), socket);
    return ssl_ptr;
}


// Умный указатель на объект X509
using X509_ptr = unique_ptr<X509, decltype(&::X509_free)>;
// Конструктор объектов типа X509_ptr
inline X509_ptr get_X509_ptr() { return X509_ptr(X509_new(), ::X509_free); }



/* ====================================== *
 * Инициализация и генерация
 *      Описание смотрите над реализацией
 * ====================================== */
void initializeOpenSSL();
EVP_PKEY_ptr generatePrivateKey();
EVP_PKEY_ptr generatePrivateKey(u_int keySize);
X509_ptr generateSertificate(EVP_PKEY_ptr& evp_pkey_ptr);

/* ====================================== *
 * Установка атрибутов
 *      Описание смотрите над реализацией
 * ====================================== */
bool setPrivateKey(SSL_CTX_ptr& ssl_ctx_ptr, EVP_PKEY_ptr& evp_pkey_ptr);
bool setPrivateKey(SSL_CTX_ptr& ssl_ctx_ptr, string& pkey_path);
bool setSertificate(SSL_CTX_ptr& ssl_ctx_ptr, X509_ptr& x509_ptr);
bool setSertificate(SSL_CTX_ptr& ssl_ctx_ptr, string& sertificate_path);
bool checkPrivateKey(SSL_CTX_ptr& ssl_ctx_ptr);

/* ======================= *
 * Перегруженные операторы
 *      Для удобства.
 * ======================= */
int operator >> (SSL_ptr& ssl_ptr, string& str);
int operator << (SSL_ptr& ssl_ptr, const string& str);

/* ============================= *
 * Примеры:
 *      TCP клиент/сервер - TLS
 *      UDP клиент/сервер - DTLS
 * ============================= */
bool serverExample_TLS(u_short port);
bool clientExample_TLS(string serverAddres, u_short port);
bool serverExample_DTLS(u_short port);
bool clientExample_DTLS(string serverAddres, u_short port);



int main()
{
    initializeOpenSSL();

    string connectType;
    string programmType;

    cout << "Enter programm type[server/client]:> ";
    getline(cin, programmType);

    if (programmType == "server")
    {
        cout << "Start server!\n" << endl;
        serverExample_TLS(25565);
    }
    else if (programmType == "client")
    {
        cout << "Start client!\n" << endl;
        clientExample_TLS("127.0.0.1", 25565);
    }

    return 0;
}


/* ============================ *
 * Пример простого TLS сервера.
 * ============================ */
bool serverExample_TLS(u_short port)
{
    // Создаём контекст SSL
    cout << "Create ssl context... ";
    SSL_CTX_ptr ssl_ctx_ptr = get_SSL_CTX_ptr(SSLv23_server_method());
    if (!ssl_ctx_ptr)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // генерируем закрытый ключ
    cout << "Generate private key... ";
    EVP_PKEY_ptr privateKey = generatePrivateKey(2048);
    if (!privateKey)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Генерируем сертефикат
    cout << "Generate sertificate X.509... ";
    X509_ptr sertificate = generateSertificate(privateKey);
    if (!sertificate)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Устанавливаем ключ и сертефикат
    cout << "Set private key... ";
    if (!setPrivateKey(ssl_ctx_ptr, privateKey))
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;
    cout << "Set sertificate... ";
    if (!setSertificate(ssl_ctx_ptr, sertificate))
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Проверяем, всё ли в порядке
    cout << "Check private key... ";
    if (!checkPrivateKey(ssl_ctx_ptr))
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Получаем сокет
    cout << "Create socket... ";
    int serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!serverSocket)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Заполняем структуру sockaddr_in
    sockaddr_in server_addres;
    // Очищаем структуру
    memset(&server_addres, 0, sizeof(server_addres));
    server_addres.sin_family = AF_INET;   // IPv4
    server_addres.sin_port = htons(port); // Port
    server_addres.sin_addr.s_addr = htonl(INADDR_ANY);

    // Указываем, что сокеты могут быть переиспользованы.
    bool reuseAddr = true;
    ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

    // Биндим сокет
    cout << "Bind socket... ";
    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(::bind(serverSocket, pSockaddr(&server_addres), sizeof(server_addres)) < 0)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Устанавливаем очередь
    ::listen(serverSocket, 5);

    while(true)
    {
        // Ожидаем подключения
        cout << "Whait connect... " << std::flush;
        sockaddr_in clientAddres;
        socklen_t sockLen = sizeof(clientAddres);
        int clientSocket = accept(serverSocket, pSockaddr(&clientAddres), &sockLen);
        // Проверка
        if (!clientSocket)
        {
            cout << "Error!" << endl;
            continue;
        }
        cout << "Accept!" << endl;

        // Создаём объект SSL соединения
        cout << "Create ssl connect... ";
        SSL_ptr sslConnect = get_SSL_ptr(ssl_ctx_ptr, clientSocket);
        cout << "Done!" << endl;

        // Ожидаем SSL соединения с клиентом
        cout << "Whait ssl connect... ";
        if (SSL_accept(sslConnect.get()) <= 0)
        {
            cout << "Error!" << endl;
            continue;
        }
        cout << "Accept!" << endl;

        // Тут начинается общение с клиентом!
        // Читаем сообщение клиента...
        string clientMessage;
        cout << "Reading client message... ";
        if ((sslConnect >> clientMessage) <= 0)
        {
            cout << "Error!" << endl;
            continue;
        }
        cout << "\"" << clientMessage << "\"" << endl;

        // Отвечаем клиенту...
        cout << "Sending message to client... ";
        if ((sslConnect << "Hello, client!") <= 0)
        {
            cout << "Error!" << endl;
            continue;
        }
        cout << "Done!" << endl;

        cout << "Shut down this connect!\n" << endl;
    }

    return true;
}

/* ============================ *
 * Пример простого TLS клиента.
 * ============================ */
bool clientExample_TLS(string serverAddres, u_short port)
{
    // Создаём контекст SSL
    cout << "Create ssl context... ";
    SSL_CTX_ptr ssl_ctx_ptr = get_SSL_CTX_ptr(SSLv23_client_method());
    if (!ssl_ctx_ptr)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Получаем сокет
    cout << "Create socket... ";
    int clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!clientSocket)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Заполняем структуру sockaddr_in
    sockaddr_in server_addres;
    // Очищаем структуру
    memset(&server_addres, 0, sizeof(server_addres));
    server_addres.sin_family = AF_INET;   // IPv4
    server_addres.sin_port = htons(port); // Port

    // Устанавливаем IP аддрес сервера
    cout << "Set IP addres... ";
    if (inet_pton(AF_INET, serverAddres.c_str(), &(server_addres.sin_addr)) <= 0)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Устанавливаем соединение с сервером
    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    cout << "Connect to server... ";
    if(connect(clientSocket, pSockaddr(&server_addres), sizeof(server_addres)) != 0)
    {
        cout << "Error!" << endl;
        return false;;
    }
    cout << "Done!" << endl;

    // Создаём объект SSL соединения
    cout << "Create ssl connect... ";
    SSL_ptr sslConnect = get_SSL_ptr(ssl_ctx_ptr, clientSocket);
    cout << "Done!" << endl;

    // Ожидаем SSL соединения с сервером
    cout << "Whait ssl connect... ";
    if (SSL_connect(sslConnect.get()) <= 0)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Connect!" << endl;


    // Тут начинается общение с сервером!
    // Отправляем сообщение серверу...
    cout << "Sending message to server... ";
    if ((sslConnect << "Hello, server!") <= 0)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "Done!" << endl;

    // Получаем ответ сервера...
    string serverMessage;
    cout << "Reading server message... ";
    if ((sslConnect >> serverMessage) <= 0)
    {
        cout << "Error!" << endl;
        return false;
    }
    cout << "\"" << serverMessage << "\"" << endl;


    cout << "Shut down this connect!" << endl;
    return true;
}

/* ============================ *
 * Пример простого DTLS сервера.
 * ============================ */
bool serverExample_DTLS(u_short port)
{
    return true;
}

/* ============================ *
 * Пример простого DTLS клиента.
 * ============================ */
bool clientExample_DTLS(string serverAddres, u_short port)
{
    return true;
}



/* ========================================================= *
 * initializeOpenSSL - инициализирует компоненты OpenSSL API
 * ========================================================= */
void initializeOpenSSL()
{
    SSL_load_error_strings();

    OpenSSL_add_all_algorithms();
    SSL_library_init();
}

/* ============================================= *
 * generatePrivateKey - генерирует закрытый ключ
 * ============================================= */
EVP_PKEY_ptr generatePrivateKey()
{
    return generatePrivateKey(1024);
}
// Тут можно указать размер ключа в битах
EVP_PKEY_ptr generatePrivateKey(u_int keySize)
{
    // Объявляем указатель на приватный ключ
    EVP_PKEY* privateKey = nullptr;
    // Объявляем указатель на контекст приватного ключа и получаем этот контекст.
    // Он будет использован для генерации ключа
    // Шифруем алгоритмом RSA
    EVP_PKEY_CTX* privateKeyContext = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    // Инициализируем контекст ключа
    EVP_PKEY_keygen_init(privateKeyContext);
    // Устанавливаем размерность генерируемого ключа
    EVP_PKEY_CTX_set_rsa_keygen_bits(privateKeyContext, keySize);
    // Генерируем ключ
    EVP_PKEY_keygen(privateKeyContext, &privateKey);

    // Освобождаем контекст приватного ключа
    EVP_PKEY_CTX_free(privateKeyContext);

    // Оборачиваем готовый приватный ключ и возвращаем его
    return EVP_PKEY_ptr(privateKey, ::EVP_PKEY_free);
}

/* ========================================================== *
 * generateSertificate - на основе закрытого ключа генерируем
 *      сертификат с открытым ключом.
 * ========================================================== */
X509_ptr generateSertificate(EVP_PKEY_ptr& evp_pkey_ptr)
{
    // Создаём умный указатель на объект типа x509
    X509_ptr x509 = get_X509_ptr();
    // Устанавливаем версию сертефиката
    X509_set_version(x509.get(), 2);
    // Делаем какую-то херню
    ASN1_INTEGER_set(X509_get_serialNumber(x509.get()), 0);
    // Устанавливаем срок действия сертефиката
    X509_gmtime_adj(X509_get_notBefore(x509.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(x509.get()), (long)60*60*24*365);
    // УЗаписываем открытый ключ, полученый из закрытого
    X509_set_pubkey(x509.get(), evp_pkey_ptr.get());

    // Заполняем сертификат данными (каб было)
    // Получаем структуру, через которую будем записывать.
    X509_NAME* name = X509_get_subject_name(x509.get());
    // Записываем данные ( не обязательно, всё равно херня выйдет )
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char*)"BY", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (const unsigned char*)"Polotsk State University", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char*)"Super student!", -1, -1, 0);
    // Устанавливаем в наш сертефикат записанные данные
    X509_set_issuer_name(x509.get(), name);
    // Подписываем
    X509_sign(x509.get(), evp_pkey_ptr.get(), EVP_md5());

    return x509;
}



/* ================================================================== *
 * setPrivateKey - устанавливает приватный ключ для шифрования данных
 * Параметры:
 *      SSL_CTX_ptr - Контекст OpenSSL
 *      EVP_PKEY_ptr - Закрытый ключ.
 * Описание:
 *      Данная функция устанавливает закрытый ключ в SSL_CTX
 * ================================================================== */
bool setPrivateKey(SSL_CTX_ptr& ssl_ctx_ptr, EVP_PKEY_ptr& evp_pkey_ptr)
{
    // Устанавливаем закрытый ключ
    if (SSL_CTX_use_PrivateKey(ssl_ctx_ptr.get(), evp_pkey_ptr.get()) <= 0)
    {
        return false;
    }
    return true;
}

/* ============================================================================== *
 * Параметры:
 *      SSL_CTX_ptr - Контекст OpenSSL
 *      string - путь к файлу с закрытым ключом.
 * Описание:
 *      Данная функция устанавливает закрытый ключ, считанный из файла, в SSL_CTX
 * ============================================================================== */
bool setPrivateKey(SSL_CTX_ptr& ssl_ctx_ptr, string& pkey_path)
{
    // Устанавливаем закрытый ключ из файла
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx_ptr.get(), pkey_path.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        return false;
    }
    return true;
}



/* ====================================================== *
 * setSertificate - устанавливает сертефикат
 * Параметры:
 *      SSL_CTX_ptr - Контекст OpenSSL
 *      X509_ptr - Сертефикат X509.
 * Описание:
 *      Данная функция устанавливает сертификат в SSL_CTX
 * ====================================================== */
bool setSertificate(SSL_CTX_ptr& ssl_ctx_ptr, X509_ptr& x509_ptr)
{
    // Устанавливаем сертефикат
    if (SSL_CTX_use_certificate(ssl_ctx_ptr.get(), x509_ptr.get()) <= 0)
    {
        return false;
    }
    return true;
}

/* ============================================================================= *
 * Параметры:
 *      SSL_CTX_ptr - Контекст OpenSSL
 *      string - Путь к файлу-сертефикату.
 * Описание:
 *      Данная функция устанавливает сертификат, загруженный из файла, в SSL_CTX
 * Примичание:
 *      Сертификат должен быть сгенерирован по стандарту X.509
 * ============================================================================= */
bool setSertificate(SSL_CTX_ptr& ssl_ctx_ptr, string& sertificate_path)
{
    // Устанавливаем сертефикат из файла
    if (SSL_CTX_use_certificate_file(ssl_ctx_ptr.get(), sertificate_path.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        return false;
    }
    return true;
}

/* ================================================================== *
 * checkPrivateKey - проверка
 * Параметры:
 *      SSL_CTX_ptr - Контекст OpenSSL
 * Описание:
 *      Функция проверяет, соответствует ли закрытый ключ сертефикату
 * ================================================================== */
bool checkPrivateKey(SSL_CTX_ptr& ssl_ctx_ptr)
{
    // Проверяем на соглассованность
    if (!SSL_CTX_check_private_key(ssl_ctx_ptr.get()))
    {
        return false;
    }
    return true;
}


/* ============================================================================= *
 * operator >>
 * Используется для чтения данных из буфера в строку
 * Возвращает количество считанных байт данных.
 *
 * Пример использования:
 *      std::string str;
 *      int bytes = ssl_ptr >> str;
 *      std::cout << "Readed " << bytes << "bts! Message: " << str << std::endl;
 * ============================================================================= */
int operator >> (SSL_ptr& ssl_ptr, string& str)
{
    // Размер временного буфера.
    const int bufSize = 1024;
    // Количество считанных байт.
    int result = 0;
    // Строковый поток. Заранее имеет какой-то определённый размер, который может быть увеличен.
    // Нужен для предотвращения постоянного копирования обычных строк.
    stringstream stringStream;

    // Цикл чтания данных
    while (true)
    {
        // Количество считанных байт
        int dataSize = 0;
        // Буффер для данных
        char buf[bufSize + 1] = { 0 };

        // Чтение данных в буффер. В dataSize будет записано количество считанных байт.
        dataSize = SSL_read(ssl_ptr.get(), buf, sizeof(char) * bufSize);
        if (dataSize < 0)
        {
            str = stringStream.str();
            return -1;
        }

        result += dataSize;
        stringStream << buf;
        if (dataSize < bufSize)
        {
            str = stringStream.str();
            return result;
        }
    }
}

/* ===================================================================== *
 * operator <<
 * Используется для записи данных из строки в буфер для передачи данных.
 * Возвращает количество записанных байт данных.
 *
 * Пример использования:
 *      std::string str = "It's message! Hello!";
 *      int bytes = ssl_ptr << str;
 * ===================================================================== */
int operator << (SSL_ptr& ssl_ptr, const string& str)
{
    // Отправляем строку.
    return ::SSL_write(ssl_ptr.get(), str.c_str(), sizeof(char) * str.length());
}
