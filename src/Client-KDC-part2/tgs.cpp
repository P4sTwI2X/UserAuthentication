#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "EncryptionUtils.h"
#include <vector>
#include <sstream>
#include <random>

using namespace std;

// user format
const string USER[5] = {"user1", "user2", "user3", "user4", "user5"};

class tgsRes
{ // encrypted with TGS session key
public:
    string serviceID;
    int timestamp; //(UNIX, second)
    int tgsLifetime;
    string serviceSessionKey;

    string convertMessage()
    {
        return serviceID + "," + to_string(timestamp) + "," + to_string(tgsLifetime) + "," + serviceSessionKey;
    }
};

class ServiceTicket {
public:
    string userID;
	string serviceID;
	int timestamp;
	int userPort;
	int serviceTicketLifetime;
	string serviceSessionKey;

    string convertMessage() {
        return userID + "," + serviceID + "," + to_string(timestamp) + to_string(userPort) + to_string(serviceTicketLifetime) + serviceSessionKey;
    }
};

int generateTgsLifetime(int &ticketLifetime)
{
    time_t now = time(nullptr);
    return now + ticketLifetime;
}

string generateServiceSessionKey(int length) {
    // define characters for the service session key
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    // seed the random number generator
    random_device rd;
    mt19937 generator(rd());

    // define a distribution for selecting characters
    uniform_int_distribution<int> distribution(0, charset.size() - 1);

    // generate the service session key
    string key;
    for (int i = 0; i < length; i++) {
        key += charset[distribution(generator)];
    }

    return key;
}

vector<string> splitFields(const string& str) {
    vector<string> fields;
    stringstream ss(str);
    string field;
    while (getline(ss, field, ','))
    {
        fields.push_back(field);
    }
    return fields;
}

bool validateRequest(const string &decryptedUA)
{
    if (decryptedUA.empty())
    {
        cerr << "Authentication failed: Empty User Authentication\n";
        return false;
    }
    vector<string> fields = splitFields(decryptedUA);


    const int expectedField = 2;
    if (fields.size() != expectedField)
    {
        cerr << "Authentication failed: Invalid number of fields in User Authentication\n";
        return false;
    }

    for (int i = 0; i < 5; i++) {
        if (fields[0] == USER[i])
            return true;
    }

    return false;
}

void receiveAndSendMsg()
{
    int port = 8080;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    // listening to the assigned socket
    listen(serverSocket, 5);

    // accepting connection request
    // struct sockaddr_in clientAddress;
    // socklen_t clientAddrLen = sizeof(clientAddress);
    // int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    // recieving data
    char tgtBuffer[1024] = {0};
    char reqBuffer[1024] = {0};
    char uaBuffer[1024] = {0};

    recv(clientSocket, tgtBuffer, 1024, 0);
    recv(clientSocket, reqBuffer, 1024, 0);
    recv(clientSocket, uaBuffer, 1024, 0);

    // decrypt tgt and UA for validation
    // string decryptedTGT = decrypt(string(tgtBuffer), "TGS_SECRET_KEY");
    // string decryptedUA = decrypt(string(auBuffer), "TGS_SESSION_KEY");

    // validate request
    bool isAuthenticated = validateRequest(string(uaBuffer));
    if (!isAuthenticated) {
        cerr << "Authentication failed";
        return;
    }

    // initial tgsRes and Service Ticket
    tgsRes res;
    ServiceTicket st;

    // split fields
    vector<string> reqFields = splitFields(reqBuffer);
    vector<string> uaFields = splitFields(uaBuffer);
    vector<string> tgtFields = splitFields(tgtBuffer);
    
    // set value for TGS response
    string serviceSessionKey = generateServiceSessionKey(12);
    int ticketLifetime = stoi(reqFields[1]); 
    int timestamp = generateTgsLifetime(ticketLifetime);
    res.serviceID = reqFields[0];
    res.timestamp = timestamp;
    res.tgsLifetime = ticketLifetime;
    res.serviceSessionKey = serviceSessionKey;
    string resMsg = res.convertMessage();

    // set value for Service Ticket
    int userPort = stoi(tgtFields[0]);
    st.serviceID = reqFields[0];
    st.userID = uaFields[0];
    st.timestamp = timestamp;
    // st.userPort = ntohs(clientAddress.sin_port);
    st.userPort = userPort;
    st.serviceSessionKey = serviceSessionKey;
    st.serviceTicketLifetime = ticketLifetime;
    string stMsg = st.convertMessage();

    send(serverSocket, resMsg.data(), resMsg.length(), 0);
    send(serverSocket, stMsg.data(), stMsg.length(), 0);

    cout << tgtBuffer << "\n";
    cout << reqBuffer << "\n";
    cout << uaBuffer << "\n";
}

int main()
{
    receiveAndSendMsg();
    // cout << "Message from client: \n" << msg[0] << "\n" << msg[1] << "\n" << msg[2] << "\n";
    // decrypt tgt using its secret key

    return 0;
}