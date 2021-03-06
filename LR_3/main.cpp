#include "stdafx.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>

struct CryptoProv {
	HCRYPTPROV hCryptProv = 0;
	bool status = false;

	CryptoProv(LPCWSTR szContainer, LPCWSTR szProvider, DWORD dwProvType, DWORD dwFlags) 
	{
		status = ::CryptAcquireContextW(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0/*CRYPT_VERIFYCONTEXT*/);
	}
	~CryptoProv() 
	{
		::CryptReleaseContext(hCryptProv, 0);
	}

	operator HCRYPTPROV() const
	{
		return hCryptProv;
	}
	operator bool() const
	{
		return status;
	}
};


bool encryptData(std::string inputFilePath, std::string outputFilePath, std::string outputKey);
bool decryptData(std::string inputFilePath, std::string outputFilePath, std::string inputKey);


int main(int argc, char** argv)
{
	if ( argc == 1 )
	{
		std::cout << "How use this programm:\n"
			<< "* If you want encrypting file:\n"
			<< "  :~$ %ProgramName% encrypt %InputFilePath% %OutputFilePath% %OutputKeyPath%\n"
			<< "* If you want decrypting file:\n"
			<< "  :~$ %ProgramName% decrypt %InputFilePath% %OutputFilePath% %InputKeyPath%"
			<< std::endl;
		return 0;
	}
	else if ( argc != 5 )
	{
		std::cout << "Invalid amount of arguments!" << std::endl;
		return -1;
	}

	std::string mode = argv[1];
	std::string inputFilePath = argv[2];
	std::string outputFilePath = argv[3];
	std::string keyFilePath = argv[4];

	if ( mode == "encrypt" )
	{
		std::cout << "== Start encrypt file ==\n" << std::endl;
		if ( !encryptData(inputFilePath, outputFilePath, keyFilePath) )
		{
			std::cout << "\n== Could not encrypt the file! ==" << std::endl;
			return -1;
		}
	}
	else if ( mode == "decrypt" )
	{
		std::cout << "== Start encrypt file ==\n" << std::endl;
		if ( !decryptData(inputFilePath, outputFilePath, keyFilePath) )
		{
			std::cout << "\n== Could not decrypt the file! ==" << std::endl;
			return -1;
		}
	}
	else
	{
		std::cout << "Invalid argument of crypt mode!" << std::endl;
		return -1;
	}
    return 0;
}


bool encryptData(std::string inputFilePath, std::string outputFilePath, std::string outputKey)
{
	std::cout << "[ENCRYPT]:> Create crypto-provider... " << std::flush;
	CryptoProv cryptProv(L"=PisosPro=", MS_ENHANCED_PROV, PROV_RSA_FULL, 0/*CRYPT_VERIFYCONTEXT*/);
	if ( !cryptProv )
	{
		std::cout << "ERROR!" << std::endl;
		return false;
	}
	std::cout << "OK!" << std::endl;


	std::cout << "[ENCRYPT]:> Generate keys... " << std::flush;
	HCRYPTKEY hSessionKey;
	if ( !CryptGenKey(cryptProv, CALG_3DES_112, CRYPT_EXPORTABLE | CRYPT_ENCRYPT | CRYPT_DECRYPT, &hSessionKey) )
	{
		std::cout << "ERROR!" << std::endl;
		return false;
	}
	std::cout << "OK!" << std::endl;


	std::cout << "[ENCRYPT]:> Encrypt... " << std::flush;
	const DWORD inputDataSize = 1048576; // 1МБ
	DWORD inputArrayBuferSize = inputDataSize;

	CryptEncrypt(hSessionKey, 0, true, 0, 0, &inputArrayBuferSize, inputDataSize);

	std::vector<BYTE> inputArrayBufer(inputArrayBuferSize, 0);

	std::ifstream fin(inputFilePath, std::ios::binary);
	std::ofstream fout(outputFilePath, std::ios::binary);
	if ( !fin || !fout )
	{
		std::cout << "ERROR (Can't open files)!" << std::endl;
		return false;
	}

	bool readedAllData = fin.eof();
	do 
	{
		fin.read((char*)inputArrayBufer.data(), inputDataSize);
		DWORD bytsReadedCount = fin.gcount();

		readedAllData = fin.eof();

		if ( !CryptEncrypt(hSessionKey, 0, readedAllData, 0, inputArrayBufer.data(), &bytsReadedCount, inputArrayBuferSize) )
		{
			std::cout << "ERROR (On encrypt)!" << std::endl;
			return false;
		}

		fout.write((char*)inputArrayBufer.data(), bytsReadedCount);

	} while (!readedAllData);

	fin.close();
	fout.close();
	std::cout << "OK!" << std::endl;


	std::cout << "[ENCRYPT]:> Export key to file... " << std::flush;
	HCRYPTKEY hPublicKey;
	if (!CryptGetUserKey(cryptProv, AT_KEYEXCHANGE, &hPublicKey))
	{
		DWORD err = GetLastError();
		if ( (err != NTE_NO_KEY) || (!CryptGenKey(cryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hPublicKey)) )
		{
			std::cout << "ERROR (On get user key)!" << std::endl;
			return false;
		}
	}

	DWORD keyBufferSize = 0;
	CryptExportKey(hSessionKey, hPublicKey, SIMPLEBLOB, 0, 0, &keyBufferSize);

	std::vector<BYTE> key(keyBufferSize, 0);
	if ( !CryptExportKey(hSessionKey, hPublicKey, SIMPLEBLOB, 0, key.data(), &keyBufferSize) )
	{
		std::cout << "ERROR (On export key to memmory)!" << std::endl;
		return false;
	}

	fout.open(outputKey, std::ios::binary);
	if ( !fout )
	{
		std::cout << "ERROR (On open file)!" << std::endl;
		return false;
	}
	fout.write((char*)key.data(), keyBufferSize);
	fout.close();

	std::cout << "OK!" << std::endl;

	if ( hSessionKey )
	{
		CryptDestroyKey(hSessionKey);
	}
	if ( hPublicKey )
	{
		CryptDestroyKey(hPublicKey);
	}

	return true;
}


bool decryptData(std::string inputFilePath, std::string outputFilePath, std::string inputKey)
{
	std::ifstream fin;
	std::ofstream fout;

	std::cout << "[DECRYPT]:> Create crypto-provider... " << std::flush;
	CryptoProv cryptProv(L"=PisosPro=", MS_ENHANCED_PROV, PROV_RSA_FULL, 0/*CRYPT_VERIFYCONTEXT*/);
	if ( !cryptProv )
	{
		std::cout << "ERROR!" << std::endl;
		return false;
	}
	std::cout << "OK!" << std::endl;


	std::cout << "[DECRYPT]:> Import key from file... " << std::flush;
	fin.open(inputKey, std::ios::binary);
	if ( !fin )
	{
		std::cout << "ERROR (On open file)!" << std::endl;
		return -1;
	}

	std::vector<BYTE> keyData;
	keyData.reserve(255);
	BYTE byte = 0;
	while ( fin.read((char*)&byte, sizeof(BYTE)) )
	{
		keyData.push_back(byte);
	}
	fin.close();

	HCRYPTKEY importedKey;
	if ( !CryptImportKey(cryptProv, (const BYTE*)keyData.data(), keyData.size(), 0, 0, &importedKey) )
	{
		std::cout << "ERROR (On import key)!" << std::endl;
		return -1;
	}
	std::cout << "OK!" << std::endl;


	std::cout << "[DECRYPT]:> Decrypt file... " << std::flush;
	const DWORD inputDataSize = 1048576; // 1МБ
	std::vector<BYTE> inputArrayBufer(inputDataSize, 0);

	fin.open(inputFilePath, std::ios::binary);
	fout.open(outputFilePath, std::ios::binary);
	if ( !fin || !fout )
	{
		std::cout << "ERROR (On open files)!" << std::endl;
		return false;
	}

	bool readedAllData = fin.eof();
	do
	{
		fin.read((char*)inputArrayBufer.data(), inputDataSize);
		DWORD bytsReadedCount = fin.gcount();

		readedAllData = fin.eof();

		if ( !CryptDecrypt(importedKey, 0, readedAllData, 0, inputArrayBufer.data(), &bytsReadedCount) )
		{
			std::cout << "ERROR (On decrypt)!" << std::endl;
			return false;
		}

		fout.write((char*)inputArrayBufer.data(), bytsReadedCount);

	} while ( !readedAllData );

	fin.close();
	fout.close();
	std::cout << "OK!" << std::endl;

	if ( importedKey )
	{
		CryptDestroyKey(importedKey);
	}

	return true;
}
