// C++ program to illustrate the client application in the 
// socket programming 
#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>

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

void user_connect(int clientSocket, int port){
    // specifying address 
	sockaddr_in serverAddress; 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(port); 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 

	// sending connection request 
	connect(clientSocket, (struct sockaddr*)&serverAddress, 
			sizeof(serverAddress)); 
}

int main() 
{
    // creating socket 
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
    int as_port = 8080;
    user_connect(clientSocket, as_port);

    // TGT ticket
    tgtReq user_tgt;
    user_tgt.userID = "A83D873";
    user_tgt.serviceID = "FETORD";
    user_tgt.userPort = clientSocket;
    user_tgt.tgtLifetime = 60;

	// sending data 
    std::string temp_msg = user_tgt.convert_message();
    std::cout << temp_msg << '\n';
	send(clientSocket, temp_msg.data(), temp_msg.length(), 0); 

	// closing socket 
	close(clientSocket); 

	return 0; 
}
