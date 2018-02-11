#ifndef AESALGORITHM_H
#define AESALGORITHM_H

#include <memory>
#include <openssl/aes.h>

class AESAlgorithm
{
private:
    unsigned int _keyLenth = 128;

    AES_KEY _aes_encryptKey;
    AES_KEY _aes_decryptKey;
    bool _validKey = false;

public:
    AESAlgorithm();
    ~AESAlgorithm();

    bool setHexKey(std::string key, unsigned int keyLenth = 128);

    std::string encrypt(const std::string& string);
    std::string decrypt(const std::string& string);

    bool encryptSmallFile(const std::string& pathInput, const std::string& pathOutput);
    bool decryptSmallFile(const std::string& pathInput, const std::string& pathOutput);
};

#endif // AESALGORITHM_H
