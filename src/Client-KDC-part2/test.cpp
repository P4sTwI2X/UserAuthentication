#include <iostream>
#include <string>
#include "EncryptionUtils.h"

int main() {
    std::string message = "Hello, World! 123";
    std::string key = "SecretKey";

    std::string encrypted_message = encrypt(message, key);
    std::cout << "Encrypted: ";
    for (char c : encrypted_message) std::cout << std::hex << (int)c << " "; // Display in hex
    std::cout << std::endl;

    std::string decrypted_message = decrypt(encrypted_message, key);
    std::cout << "Decrypted: " << decrypted_message << std::endl;

    return 0;
}