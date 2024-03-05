#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "EncryptionUtils.h"


class TicketGrantingServer {
public:
    TicketGrantingServer(int port) : port_(port) {
        unsigned char key_data[] = "0123456789abcdef0123456789abcdef"; // Example key
        EncryptionUtils::initializeAESKey(enc_key, dec_key, key_data);
        setupSocket();
    }

    ~TicketGrantingServer() {
        close(server_fd_);
    }

    void listenForRequests() {
        if (listen(server_fd_, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        std::cout << "TGS listening on port " << port_ << std::endl;

        while (true) {
            std::cout << "Waiting for connections..." << std::endl;
            if ((new_socket_ = accept(server_fd_, (struct sockaddr *)&address_, (socklen_t*)&addrlen_))<0) {
                perror("accept");
                continue;
            }
            char buffer[1024] = {0};
            read(new_socket_, buffer, 1024);
            std::cout << "Message from user: " << buffer << std::endl;
            handleRequests();
            close(new_socket_);
        }
    }

private:
    AES_KEY enc_key, dec_key;
    int server_fd_, new_socket_, port_;
    struct sockaddr_in address_;
    int opt_ = 1;
    int addrlen_ = sizeof(address_);
    

    void setupSocket() {
        if ((server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_, sizeof(opt_))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address_.sin_family = AF_INET;
        address_.sin_addr.s_addr = INADDR_ANY;
        address_.sin_port = htons(port_);

        if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
    }

    
    void handleRequests() {
        // Accept connections and handle them
        unsigned char received_data[1024]; // Adjust size as necessary
        unsigned char decrypted_data[1024];
        unsigned char encrypted_response[1024];
        const char* response = "ServiceKey123"; // Example response

        // Decrypt received data
        EncryptionUtils::AES_decrypt(received_data, decrypted_data, dec_key);
        std::cout << "Decrypted message: " << decrypted_data << std::endl;

        // Encrypt and send response
        EncryptionUtils::AES_encrypt((unsigned char*)response, encrypted_response, enc_key);
        // Send encrypted_response
        send(new_socket_, encrypted_response, sizeof(encrypted_response), 0);
        std::cout << "Service key sent to user." << std::endl;
    }
};

int main() {
    TicketGrantingServer tgs(8080);
    tgs.listenForRequests();
    return 0;
}