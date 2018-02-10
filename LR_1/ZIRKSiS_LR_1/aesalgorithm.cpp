#include "aesalgorithm.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <openssl/rand.h>

AESAlgorithm::AESAlgorithm()
{
}
AESAlgorithm::~AESAlgorithm()
{
}


bool AESAlgorithm::setHexKey(std::string key, unsigned int keyLenth)
{
    int arrayLenth = keyLenth / 8;
    // Крафтим массив для хранения ключа (Почему не просто new? Потому что почитайте о RAII)
    std::unique_ptr<unsigned char[]> keyArray(new unsigned char(arrayLenth));

    // Перевод ключа и строки в массив
    for (int i = 0; i < arrayLenth; i++) {
        keyArray[i] = std::stoi(std::string() + key[i*2] + key[i*2 + 1], nullptr, 16);
    }

    // Крафтим ключи
    if ((AES_set_encrypt_key(keyArray.get(), keyLenth, &_aes_encryptKey) != 0)
            || (AES_set_decrypt_key(keyArray.get(), keyLenth, &_aes_decryptKey) != 0)) {
        _validKey = false;
        return false;
    }

    // Устанавливаем атрибуты
    _validKey = true;
    _keyLenth = keyLenth;

    return true;
}

std::string AESAlgorithm::encrypt(const std::string& string)
{
    /* Расчёт размера массива.
     * 1 блок == 16 байт.
     * Если число байт данных кратно 16, то дополнительно требуется ещё один блок.
     */
    int sizeOfArrayForOutput = ((string.length() + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    std::string res;
    res.resize(sizeOfArrayForOutput, 0);

    for (int i = 0; i < sizeOfArrayForOutput; i += 16)
        AES_ecb_encrypt((unsigned char*)string.data() + i, (unsigned char*)res.data() + i, &_aes_encryptKey, AES_ENCRYPT);

    return res;
}
std::string AESAlgorithm::decrypt(const std::string& string)
{
    // На тот самый случай, если какой-то даун(не будь как даун) отрежет кусок блока
    int sizeOfArrayForOutput = (string.length() / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    std::string res;
    res.resize(sizeOfArrayForOutput, 0);

    for (int i = 0; i < sizeOfArrayForOutput; i += 16)
        AES_ecb_encrypt((unsigned char*)string.data() + i, (unsigned char*)res.data() + i, &_aes_decryptKey, AES_DECRYPT);

    return res;
}


bool AESAlgorithm::encryptSmallFile(const std::string& pathInput, const std::string& pathOutput)
{
    std::ifstream fin(pathInput);
    std::ofstream fout(pathOutput);

    if (!fin || !fout) return false;

    std::stringstream inputBuffer;
    inputBuffer << fin.rdbuf();
    fin.close();

    fout << encrypt(inputBuffer.str());
    fout.close();

    return true;
}
bool AESAlgorithm::decryptSmallFile(const std::string& pathInput, const std::string& pathOutput)
{
    std::ifstream fin(pathInput);
    std::ofstream fout(pathOutput);

    if (!fin || !fout) return false;

    std::stringstream inputBuffer;
    inputBuffer << fin.rdbuf();
    fin.close();

    fout << decrypt(inputBuffer.str());
    fout.close();

    return true;
}
