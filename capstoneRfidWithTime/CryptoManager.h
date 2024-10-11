#ifndef CRYPTO_MANAGER_H
#define CRYPTO_MANAGER_H

#include <Crypto.h>
#include <ChaChaPoly.h>

class CryptoManager
{
public:
    String encryptMessage(const char *plaintext, const uint8_t *key, const uint8_t *nonce)
    {
        ChaChaPoly chacha;
        uint8_t ciphertext[128];
        uint8_t tag[16];

        chacha.clear();
        chacha.setKey(key, 32);
        chacha.setIV(nonce, 12);
        size_t plaintextLength = strlen(plaintext);
        chacha.encrypt(ciphertext, (uint8_t *)plaintext, plaintextLength);
        chacha.computeTag(tag, sizeof(tag));

        // Convert to hex strings
        String encryptedData = convertToHex(ciphertext, plaintextLength);
        String tagString = convertToHex(tag, sizeof(tag));

        // Create JSON payload
        String payload = "{\"ciphertext\":\"" + encryptedData + "\",\"tag\":\"" + tagString + "\"}";

        return payload;
    }

private:
    String convertToHex(const uint8_t *data, size_t length)
    {
        String hexString = "";
        for (size_t i = 0; i < length; i++)
        {
            if (data[i] < 0x10)
                hexString += "0";
            hexString += String(data[i], HEX);
        }
        return hexString;
    }
};

#endif
