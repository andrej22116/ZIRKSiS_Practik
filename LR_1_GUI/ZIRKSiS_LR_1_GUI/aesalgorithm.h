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
    void generateVector();

    std::string encrypt(std::string string);
    std::string decrypt(std::string string);

    bool encryptSmallFile(std::string pathInput, std::string pathOutput);
    bool decryptSmallFile(std::string pathInput, std::string pathOutput);
};

#endif // AESALGORITHM_H
