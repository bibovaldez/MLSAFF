#ifndef ENCRYPTION_H
#define ENCRYPTION_H


#include <Crypto.h>
#include <ChaChaPoly.h>

struct EncryptionModule {
    uint8_t key[32];
    uint8_t nonce[12];
};

class Encryption {
public:
    static String encryptMessage(const char* plaintext, const EncryptionModule& module);
    static String decryptMessage(const String& ciphertextHex, const String& tagHex, const EncryptionModule& module);

private:
    static void hexStringToBytes(const String& hexString, uint8_t* bytes);
};

#endif // ENCRYPTION_H