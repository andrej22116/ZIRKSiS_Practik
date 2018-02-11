#include <iostream>
#include "aesalgorithm.h"

int main(int argv, char** argc)
{
    auto log = [](std::string msg, bool isError = false) {
        if (isError) {
            std::cout << "[ERROR]: " << msg << std::endl;
        } else {
            std::cout << "[LOG]: " << msg << std::endl;
        }
    };

    log("Read parametrs");
    if (argv != 5) {
        log("Invalid arguments!", true);
        return -1;
    }

    std::string inputFilePath = argc[1];
    std::string key = argc[2];
    std::string mode = argc[3];
    std::string outputFilePath = argc[4];

    AESAlgorithm aes;
    if (!aes.setHexKey(key)) {
        log("Invalid key!", true);
        return -2;
    }

    if (mode == "encrypt") {
        log("Start encrypting file!");
        if (!aes.encryptSmallFile(inputFilePath, outputFilePath)) {
            log("Error in encrypt!", true);
            return -3;
        }
    } else if (mode == "decrypt") {
        log("Start decrypting file!");
        if (!aes.decryptSmallFile(inputFilePath, outputFilePath)) {
            log("Error in decrypt!", true);
            return -4;
        }
    } else {
        log("Invalid mode!", true);
        return -5;
    }

    log("Comlate! Watch file: " + outputFilePath);

    return 0;
}
