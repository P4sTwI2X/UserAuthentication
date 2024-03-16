#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <chrono>

#define AS_PORT 8080
#define TGS_PORT 8081
#define MAX_BUFFER 1048576

const std::string clientSecretKey = "clientsecretkey11223";
const std::string tgsID = "ticketgrantingserviceID"; 
const std::string tgsSecretKey = "ticketseeeeeeKEY";
const std::string serviceSecretKey = "servicesecretkey";

std::int64_t Timestamp(){
    return std::chrono::duration_cast<std::chrono::seconds>
    (std::chrono::system_clock::now().time_since_epoch()).count();
}

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

#endif