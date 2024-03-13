// C++ program to illustrate the client application in the 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include "EncryptionUtils.h"
#include <vector>

#define MAX_BUFFER 1048576

std::vector<std::string> extractData(std::string str){
	int temp_ctr = 0;
	std::vector<std::string> data;
	for(int i=0; i<str.length(); i++){
		if(str[i] == ','){
			data.push_back(str.substr(temp_ctr, i));
			temp_ctr = i+1;
		}
	}
	if(temp_ctr != str.length()){ // final element
		data.push_back(str.substr(temp_ctr, str.length()));
	}
	return data;
}

int listen_port(int thisSocket, int port){
    // specifying the address 
	sockaddr_in otherAddress; 
	otherAddress.sin_family = AF_INET; 
	otherAddress.sin_port = htons(port); 
	otherAddress.sin_addr.s_addr = INADDR_ANY; 

	// binding socket. 
	bind(thisSocket, (struct sockaddr*)&otherAddress, 
		sizeof(otherAddress)); 

	// listening to the assigned socket 
	listen(thisSocket, 5); 

	// accepting connection request 
	int clientSocket 
		= accept(thisSocket, nullptr, nullptr); 
    
    return clientSocket;
}

class tgtReq{
    public:

	std::string userID;
	std::string serviceID;
	size_t userPort; //IPv4
	size_t tgtLifetime;

    std::string convert_message(){
        std::string temp = userID + "," + serviceID + "," 
            + std::to_string(userPort) + "," + std::to_string(tgtLifetime);
        return temp;
    }
};

class clientInfo{
    public:
    std::string userID;
    std::string userPass;
    std::string serviceID;
    int userSocket;
    std::string clientSecretKey(){
        return encrypt(userID + userPass, "12346789123456789");
        //return encrypt(userID + userPass, key);
    }
} user;

void connect_port(int clientSocket, int port){
    // specifying address 
	sockaddr_in otherAddress; 
	otherAddress.sin_family = AF_INET; 
	otherAddress.sin_port = htons(port); 
	otherAddress.sin_addr.s_addr = INADDR_ANY; 

	// sending connection request 
	connect(clientSocket, (struct sockaddr*)&otherAddress, 
			sizeof(otherAddress)); 
}

std::string TGTticket_req(int as_port){
    connect_port(user.userSocket, as_port);

    // TGT Ticket request construct
    tgtReq user_tgt;
    user_tgt.userID = user.userID;
    user_tgt.serviceID = user.serviceID;
    user_tgt.userPort = user.userSocket;
    user_tgt.tgtLifetime = 60;

	// send TGT Ticket request 
    std::string temp_msg = user_tgt.convert_message();
    printf("User TGT request: %s \n", temp_msg.data());
	send(user.userSocket, temp_msg.data(), temp_msg.length(), 0); 

    // listen to AS reply
    int AS_socket = listen_port(user.userSocket, as_port);

	// recieving data 
	char buffer[1024] = { 0 }; 
	recv(AS_socket, buffer, sizeof(buffer), 0); 
	//std::cout << "Message from server: " << buffer << std::endl; 

    return std::string(buffer);
}

int main() 
{
    // user credentials
	user.userSocket = socket(AF_INET, SOCK_STREAM, 0); 
    user.userID = "USERID1234567";
    user.userPass = "cake8888";
    user.serviceID = "CAKE_COMPANY_LMD_INT";
    const int AS_PORT = 8080;
    connect_port(user.userSocket, AS_PORT); //Authentication server
    std::cout << "CSK: " << user.clientSecretKey() << std::endl;

    // 1. Request TGT ticket --------------------------
    std::string TGTticket = TGTticket_req(AS_PORT);

    // 2. Get AS reply & TGT ticket -------------------------
    listen_port(user.userSocket, AS_PORT);
    char buffer[MAX_BUFFER] = { 0 };
	recv(user.userSocket, buffer, sizeof(buffer), 0); // AS reply
    std::string ASreply_en = std::string(buffer);
    for(int i=0; i<10; i++){
		printf("%2x ",ASreply_en.data()[i]);
	}
    recv(user.userSocket, buffer, sizeof(buffer), 0); // TGT ticket
    std::string TGT_en = std::string(buffer);

    // 2a. Decrypt & check AS reply
    std::string ASrep_msg = decrypt(ASreply_en, "clientsecretkey11223");
    //printf("AS reply: %s", ASrep_msg);
    std::vector<std::string> ASreply = extractData(ASrep_msg);

	// End. Close client socket ------------------------
	close(user.userSocket); 

	return 0; 
}
