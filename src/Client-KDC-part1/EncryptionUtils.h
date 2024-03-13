#include <iostream>
#include <string>

std::string encrypt(const std::string &plaintext, const std::string &key) {
    std::string encrypted;
    for (size_t i = 0; i < plaintext.length(); ++i) {
        char keyChar = key[i % key.length()]; // Cycle through key characters
        char encChar = (plaintext[i] + keyChar) % 128; // Use % 128 to stay within ASCII
        encrypted.push_back(encChar);
    }
    return encrypted;
}

std::string decrypt(const std::string &ciphertext, const std::string &key) {
    std::string decrypted;
    for (size_t i = 0; i < ciphertext.length(); ++i) {
        char keyChar = key[i % key.length()];
        char decChar = (ciphertext[i] - keyChar + 128) % 128; // Use +128 to avoid negative numbers
        decrypted.push_back(decChar);
    }
    return decrypted;
}