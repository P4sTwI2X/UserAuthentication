// C++ program to show the example of server application in 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <ctime>
#include <random>
#include "EncryptionUtils.h"

#define MAX_BUFFER 1048576
const std::string clientSecretKey = "clientsecretkey11223";
const std::string tgsID = "ticketgrantingserviceID"; 
const std::string tgsSecretKey = "ticketseeeeeeKEY";

std::string genRandKey(int length) {
    // define characters for the service session key
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    // seed the random number generator
    std::random_device rd;
    std::mt19937 generator(rd());

    // define a distribution for selecting characters
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);

    // generate the service session key
    std::string key;
    for (int i = 0; i < length; i++) {
        key += charset[distribution(generator)];
    }

    return key;
}

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

class asReply{
	public:

	std::string tgsID;
	int timestamp = time_t(0);
	int tgtLifetime;
	std::string tgsSessionKey;

	std::string convert_message(){
        std::string temp = tgsID + "," + std::to_string(timestamp) + "," 
            + std::to_string(tgtLifetime) + "," + tgsSessionKey;
        return temp;
    }
};

class TGT{ //Ticket-granting Ticket
public:
	std::string userID;
	std::string tgsID;
	int timestamp = time_t(0);
	int userPort;
	int tgtLifetime;
	std::string tgsSessionKey;

	std::string convert_message(){
        std::string temp = userID + "," + tgsID + "," + std::to_string(timestamp) + "," 
            + std::to_string(userPort) + "," + std::to_string(tgtLifetime)
			+ "," + tgsSessionKey;
        return temp;
    }
};

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

bool TGTReq_check(std::vector<std::string> data){
	// check expected size 4
	if(data.size() != 4)
		return false;

	// check userID non null

	// check if serviceID exists in server (we don't implement that here)

	// check userPort non null

	// check TGT lifetime requested
	float time = std::stoi(data[3]);
	if(time <= 0 && time >= 900){
		printf("TGT time requested too short or long: %f", time);
		return false;
	}
}

int listen_port(int serverSocket, int port){
    // specifying the address 
	sockaddr_in serverAddress; 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(port); 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 

	// binding socket. 
	bind(serverSocket, (struct sockaddr*)&serverAddress, 
		sizeof(serverAddress)); 

	// listening to the assigned socket 
	listen(serverSocket, 5); 

	// accepting connection request 
	int clientSocket 
		= accept(serverSocket, nullptr, nullptr); 
    
    return clientSocket;
}

int main() 
{ 
	// 1. Get request from user

	// creating socket 
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
    int serverPort = 8080;
    int clientSocket = listen_port(serverSocket, serverPort);

	// recieving data 
	char buffer[MAX_BUFFER] = { 0 }; 
	recv(clientSocket, buffer, sizeof(buffer), 0); 
	std::cout << "Message from client: " << buffer << std::endl; 

	// extract data
	std::vector<std::string> tgtReqData = extractData(std::string(buffer));

	// check data validity
	if(TGTReq_check(tgtReqData)){
		std::cout << "TGT request accepted" << std::endl;
		TGT tgt;
		asReply rep;

		// construct AS_reply
		rep.tgsID = tgsID;
		rep.tgtLifetime = std::stoi(tgtReqData[2]);
		//rep.tgsSessionKey = genRandKey(32);
		rep.tgsSessionKey = "thisisthesessionkey";

		std::cout << "AS reply: " << rep.convert_message() << std::endl;

		for(int i=0; i<10; i++){
			printf("%2x ",rep.convert_message()[i]);
		}

		std::string rep_en = encrypt(rep.convert_message(), clientSecretKey);

		// construct TGT
		tgt.tgsID = tgsID;
		tgt.tgsSessionKey = rep.tgsSessionKey;
		tgt.tgtLifetime = rep.tgtLifetime;
		tgt.userID = tgtReqData[0];
		tgt.userPort = std::stoi(tgtReqData[2]);

		//std::cout << "TGT: " << tgt.convert_message() << std::endl;

		std::string tgt_en = encrypt(tgt.convert_message(), tgsSecretKey);

		// send packages
		connect_port(serverSocket, tgt.userPort);
		send(serverSocket, rep_en.data(), rep_en.length(), 0);
		send(serverSocket, tgt_en.data(), tgt_en.length(), 0); 
	}

	// closing the socket
	close(serverSocket);

	return 0; 
}
