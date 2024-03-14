// C++ program to illustrate the client application in the 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include "EncryptionUtils.h"
#include <vector>

#define AS_PORT 8080
#define MAX_BUFFER 1048576

void connect_port(int otherSocket, int port){
    // specifying address 
	sockaddr_in otherAddress; 
	otherAddress.sin_family = AF_INET; 
	otherAddress.sin_port = htons(port); 
	otherAddress.sin_addr.s_addr = INADDR_ANY; 

	// sending connection request 
	int return_code = connect(otherSocket, (struct sockaddr*)&otherAddress, 
			sizeof(otherAddress)); 
	if(return_code == -1){
        printf("Port connection error.\n");
    }
}

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
    clientInfo(){
        this->userSocket = socket(AF_INET, SOCK_STREAM, 0);
        
        if(this->userSocket == -1)
            printf("Socket creation error.\n");
    }
    ~clientInfo(){
        close(userSocket);
    }
} user;

void sendTGTticketReq(){
    // TGT Ticket request construct
    tgtReq user_tgt;
    user_tgt.userID = user.userID;
    user_tgt.serviceID = user.serviceID;
    user_tgt.userPort = AS_PORT;
    user_tgt.tgtLifetime = 60;

	// send TGT Ticket request 
    std::string temp_msg = user_tgt.convert_message();
    printf("User TGT request: %s \n", temp_msg.data());
	if(send(user.userSocket, temp_msg.data(), temp_msg.length(), 0) == -1) 
        printf("Send failed.\n");
}

std::vector<std::string> getASreply(int serverSocket){
    // get data
    int length;
    if(recv(serverSocket, &length, sizeof(int)/sizeof(char), 0) == -1)
        printf("Receive error: int.\n");
    char buffer[MAX_BUFFER] = { 0 };
	if(length >= MAX_BUFFER || recv(serverSocket, buffer, length, 0) == -1) // AS reply
        printf("Receive error.\n");
    std::string ASreply_en = std::string(buffer);
    //std::cout << ASreply_en << std::endl;
    
    // decrypt and extract reply
    std::string ASrep_msg = decrypt(ASreply_en, "clientsecretkey11223");
    std::vector<std::string> ASreply = extractData(ASrep_msg);

    std::cout << "AS reply: " << ASrep_msg << std::endl;

    return ASreply;
}

std::string getTGT_en(int serverSocket){
    //get data
    char buffer[MAX_BUFFER] = {0};
    if(recv(serverSocket, buffer, sizeof(buffer), 0) == -1) // TGT ticket
        printf("Receive error.\n");
    std::string TGT_en = std::string(buffer);

    return TGT_en;
}

int main() 
{
    // user credentials
    user.userID = "USERID1234567";
    user.userPass = "cake8888";
    user.serviceID = "CAKE_COMPANY_LMD_INT";
    //std::cout << "CSK: " << user.clientSecretKey() << std::endl;

    // connect ports
    connect_port(user.userSocket, AS_PORT);

    // 1. Send request for TGT ticket --------------------------
    sendTGTticketReq();

    // 2. Get AS reply & TGT ticket -------------------------
    std::vector<std::string> ASrep = getASreply(user.userSocket);
    std::string TGT_en = getTGT_en(user.userSocket);

	return 0; 
}
