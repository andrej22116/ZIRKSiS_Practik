using System;
using System.IO;
using System.Text;
using System.Security.Cryptography;

namespace LR_4
{
    class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length == 1)
            {
                Console.WriteLine("You can use this program:\n" +
                    "<alg[aes/des]> <inputfilename> <password> <mode[encrypt/decrypt]> <outputfilename>");
                return;
            }
            if (args.Length != 5)
            {
                return;
            }

            string algorithm = args[0].ToLower();
            if (algorithm == "aes" 
                && !crypt(new AesCryptoServiceProvider() { Mode = CipherMode.ECB },
                        args[1], args[2], args[3], args[4]))
            {
                Console.WriteLine("Error on AES method!");
                return;
            }
            else if (algorithm == "des" 
                && !crypt(new DESCryptoServiceProvider() { Mode = CipherMode.ECB }, 
                        args[1], args[2], args[3], args[4]))
            {
                Console.WriteLine("Error on DES method!");
                return;
            }
        }

        static bool crypt(SymmetricAlgorithm alg, string input, string password, string mode, string output)
        {
            Console.WriteLine("CRYPTO:> Modify password...");
            byte[] key = calcPasswordHash(password, alg.KeySize);
            var iv = new byte[alg.BlockSize / 8];
            alg.Key = key;
            alg.IV = iv;

            Console.WriteLine("CRYPTO:> Create crypto transform...");
            
            mode = mode.ToLower();
            ICryptoTransform cryptoTransform;

            Stream inputStream;
            Stream outputStream;
            if (mode == "encrypt")
            {
                cryptoTransform = alg.CreateEncryptor(key, iv);
                inputStream = File.OpenRead(input);
                outputStream = new CryptoStream(File.Open(output, FileMode.Create),
                                                cryptoTransform, CryptoStreamMode.Write);
            }
            else if (mode == "decrypt")
            {
                cryptoTransform = alg.CreateDecryptor(key, iv);
                inputStream = new CryptoStream(File.OpenRead(input),
                                                cryptoTransform, CryptoStreamMode.Read);
                outputStream = new FileStream(output, FileMode.Create);
            }
            else
            {
                return false;
            }

            Console.WriteLine("CRYPTO:> Crypt...");
            if (!redirect(inputStream, outputStream))
            {
                Console.WriteLine("CRYPTO:> Error!");
                return false;
            }

            Console.WriteLine("\nDone!");
            return true;
        }

        static byte[] calcPasswordHash(string password, int keySize)
        {
            var sha = new SHA512CryptoServiceProvider();
            var hash = sha.ComputeHash(Encoding.ASCII.GetBytes(password));
            var res = new Byte[keySize / 8];
            Array.Copy(hash, res, keySize / 8);
            return res;
        }


        static bool redirect(Stream input, Stream output)
        {
            try
            {
                if (input == null || output == null)
                {
                    throw new Exception("No files!");
                }

                int bytesReaded = 0;
                var buffer = new byte[1024];
                while ((bytesReaded = input.Read(buffer, 0, buffer.Length)) > 0)
                {
                    output.Write(buffer, 0, bytesReaded);
                }

                input.Dispose();
                output.Dispose();
            }
            catch(Exception ex)
            {
                return false;
            }
            return true;
        }
    }
}
