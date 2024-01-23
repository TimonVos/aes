#include <Arduino.h>
#include <aes/esp_aes.h>
#include <esp_random.h>

#define AES_BLOCK_SIZE 16

// Function prototypes
void aes128CbcEncrypt(uint8_t *input, uint8_t *output, size_t length, const uint8_t *key, uint8_t *iv);
void aes128CbcDecrypt(uint8_t *input, uint8_t *output, size_t length, const uint8_t *key, uint8_t *iv);
void applyPkcs7Padding(uint8_t *input, size_t inputLength, size_t blockSize, size_t paddedLength);

void setup() {
    Serial.begin(115200);
    // Wait for serial to initialize.
    while (!Serial) {
        ; // wait for serial port to connect.
    }

    // Example usage
    uint8_t key[16] = {/* 16 bytes AES key */};
    uint8_t iv[16] = {/* 16 bytes IV */};

    // fill key and iv with random data
    esp_fill_random(key, 16);
    esp_fill_random(iv, 16);

    uint8_t iv_copy[16]; // Copy of the IV for decryption
    memcpy(iv_copy, iv, AES_BLOCK_SIZE); // Copy the IV

    const char* plaintext = "CHATGPT is godlike";
    size_t inputLength = strlen(plaintext);
    size_t paddedLength = ((inputLength / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

    uint8_t input[paddedLength];
    memcpy(input, plaintext, inputLength);

    uint8_t encrypted[paddedLength];
    uint8_t decrypted[paddedLength];

    // Apply PKCS#7 padding
    applyPkcs7Padding(input, inputLength, AES_BLOCK_SIZE, paddedLength);

    // Encrypt
    aes128CbcEncrypt(input, encrypted, paddedLength, key, iv);

    // Reset IV for decryption
    memcpy(iv, iv_copy, AES_BLOCK_SIZE);

    // Decrypt
    aes128CbcDecrypt(encrypted, decrypted, paddedLength, key, iv);

    // Print encrypted and decrypted data
    Serial.println("Encrypted Data:");
    for (size_t i = 0; i < paddedLength; i++) {
        Serial.printf("%02x", encrypted[i]);
    }
    Serial.println("\nDecrypted Data:");
    Serial.write(decrypted, paddedLength);
    Serial.println();
}

void loop() {
    // Your code here
}

void aes128CbcEncrypt(uint8_t *input, uint8_t *output, size_t length, const uint8_t *key, uint8_t *iv)
{
    esp_aes_context ctx;
    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key, 128);
    esp_aes_crypt_cbc(&ctx, ESP_AES_ENCRYPT, length, iv, input, output);
    esp_aes_free(&ctx);
}

void aes128CbcDecrypt(uint8_t *input, uint8_t *output, size_t length, const uint8_t *key, uint8_t *iv)
{
    esp_aes_context ctx;
    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, key, 128);
    esp_aes_crypt_cbc(&ctx, ESP_AES_DECRYPT, length, iv, input, output);
    esp_aes_free(&ctx);
}

void applyPkcs7Padding(uint8_t *input, size_t inputLength, size_t blockSize, size_t paddedLength)
{
    size_t paddingSize = blockSize - (inputLength % blockSize);
    for (size_t i = inputLength; i < paddedLength; i++)
    {
        input[i] = (uint8_t)paddingSize;
    }
}
