#pragma once

#include <string>

/**
 * @brief The EncryptionHelper class provides functions for encrypting and decrypting data.
 *
 * Responsibilities:
 * - Encrypt data before saving to a file.
 * - Decrypt data when loading from a file.
 *
 * Note:
 * - You may use libraries such as OpenSSL or Crypto++ to implement robust encryption algorithms (e.g., AES-256).
 *
 * Usage:
 * - Call encrypt() with the plaintext and key to obtain the ciphertext.
 * - Call decrypt() with the ciphertext and key to obtain the plaintext.
 */
class EncryptionHelper {
public:
    /**
     * @brief Encrypt the given plaintext using the provided key.
     * @param plainText The plaintext to encrypt.
     * @param key The encryption key.
     * @return std::string The encrypted ciphertext.
     */
    static std::string encrypt(const std::string& plainText, const std::string& key);

    /**
     * @brief Decrypt the given ciphertext using the provided key.
     * @param cipherText The ciphertext to decrypt.
     * @param key The decryption key.
     * @return std::string The decrypted plaintext.
     */
    static std::string decrypt(const std::string& cipherText, const std::string& key);
};
