#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "EncryptionUtils.h"

using namespace std;

class tgsReq {
public:
    string serviceID;
	int ticketLifetime;

    string convertMessage() {
        return serviceID + "," + to_string(ticketLifetime);
    }
};

void sendMsgToTGS(const string &tgt, tgsReq &req, const string &ua) {
    int tgs_port = 8080;
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // specifying address 
	sockaddr_in serverAddress; 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(tgs_port); 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 

	// sending connection request 
	if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cout << "Connect failed\n";
    }
    else {
        cout << "Connect successfully\n";
    }

    // Convert TGS request to string message
    const string requestMessage = req.convertMessage();
    
    // Send data
    send(clientSocket, tgt.c_str(), tgt.length(), 0);
    send(clientSocket, requestMessage.c_str(), requestMessage.length(), 0);
    send(clientSocket, ua.c_str(), ua.length(), 0);

    // Receive data
    char resBuffer[1024] = {0}, stBuffer[1024] = {0};
    recv(clientSocket, resBuffer, sizeof(resBuffer), 0);
    recv(clientSocket, stBuffer, sizeof(stBuffer), 0);
    cout << resBuffer << "\n";
    cout << stBuffer << "\n";
}

int main() {
    // send message to TGS (TGT, tgsReq, UA)
    tgsReq req;
    req.serviceID = "usertest123";
    req.ticketLifetime = 10000;
    sendMsgToTGS("user1,tgsid,10000,1080,10000,tgssessionkey", req, "user1,10000");
    return 0;
}
