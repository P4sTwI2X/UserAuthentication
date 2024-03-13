#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
using namespace std;

class UA {
    string userID;
    int timestamp;

public:

    UA(string userID, int timestamp) : userID(userID), timestamp(timestamp) {}


    string getUserID() const {
        return userID;
    }
    int getTimestamp() const {
        return timestamp;
    }
    void setUserID(const string& newUserID) {
        userID = newUserID;
    }
    void setTimestamp(int newTimestamp) {
        timestamp = newTimestamp;
    }

    void display() const {
        cout << "User ID: " << userID << ", Timestamp: " << timestamp << endl;
    }

    string createMessage(string serviceSessionKey) {
        return userID + "," + to_string(timestamp) + "," + serviceSessionKey;
    }
    //Q: return encrypt(message) ?
    //A - Thien: decrypt(message, serviceTicket.serviceSessionKey) from user
    //    Context: user creates this message, encrypt it with serviceSessionKey to send to Service Server.
    // then Service Server decrypts this after getting serviceSessionKey from decrypting Service Ticket.

    //Q: do i need to do anything else to this Object ?
    //A - Thien: [get to main fuction]
};

class ServiceAuthen {
private:
    string serviceID;
    int timestamp;

public:

    ServiceAuthen(const string& serviceID, int timestamp) : serviceID(serviceID), timestamp(timestamp) {}

    string getServiceID() const {
        return serviceID;
    }

    int getTimestamp() const {
        return timestamp;
    }

    void setServiceID(const string& newServiceID) {
        serviceID = newServiceID;
    }

    void setTimestamp(int newTimestamp) {
        timestamp = newTimestamp;
    }
    void display() const {
        cout << "Service ID: " << serviceID << ", Timestamp: " << timestamp << endl;
    }
    string createMessage(string serviceSessionKey) {
        return serviceID + "," + to_string(timestamp) + "," + serviceSessionKey;
    }
    //Q: return encrypt(message) ?
    //A - Thien: user would encrypt(message, serviceSecretKey) //from previous part, say it's just a random key

    //Q: do i need to do anything else to this Object ?
    //A - Thien: [get to main function]
};

//i don't know where to put these check function but i'll implement those for now.
bool checkUserid(string userIdTicket, string UAticket) {
    return userIdTicket == UAticket;
}

bool checkTime(int TicketStartTime, int UAtime) {
    //invalid if more tan 1 min (60)
    //return (UAtime - TicketEndTime) <= 60;
    return TicketStartTime == UAtime;
}

bool checkIP(string clientNetworkAddress, string senderIP) {
    return clientNetworkAddress == senderIP;
}

bool lifetimeCheck(int endTime) {
    //im assuming lifetime is current time
    auto now = chrono::system_clock::now();
    int unix_timestamp = chrono::system_clock::to_time_t(now);
    //im
    return endTime > unix_timestamp;
}
//Q: is this the right way to check timestamp ?
/*A - Thien: I really don't get what you mean by the lifetimeCheck() function,
    but here is an easy way to understand: there are two timestamp checks.
1. UserAuthenticator.timestamp > ServiceTicket.timestamp + fixed delay time (120 s) if yes then 
fails
    - This ticket is temporarily used for SHORT service sessions.
    - If the ticket fails and #2 still false, client can renew the ticket through KDC without having to enter credentials
    such as passwords etc.
2. ServiceTicket.timestamp + ServiceTicket.lifetime > ServiceServer.time_at if yes then fails
    - After some LONG time, user must enter credentials for a new ticket from the very beginning.
*/
// StackOverflow: https://stackoverflow.com/questions/14682153/lifetime-of-kerberos-tickets
// Watch: https://www.youtube.com/watch?v=5N242XcKAsM&t=815s&ab_channel=DestinationCertification&t=757s


//ticket structure for reference (please check if the following structure is correct and is in the right order)
struct Ticket {
    string clientID;
    string clientNetworkAddress;
    int startTime = 0;
    int endTime = 0;
    string sessionKey;
    string serviceID;
};

Ticket parseTicket(const string& str) {
    istringstream iss(str);
    Ticket ticket;
    string token;

    getline(iss, ticket.clientID, ',');
    getline(iss, ticket.clientNetworkAddress, ',');
    getline(iss, token, ',');
    ticket.startTime = stoi(token);
    getline(iss, token, ',');
    ticket.endTime = stoi(token);
    getline(iss, ticket.sessionKey, ',');
    getline(iss, ticket.serviceID, ',');

    return ticket;
}



//functions
string decrypt(const string& message);
string encrypt(const string& message);



string encrypt(const string& message) {
    //placeholder
    return message;
}

string decrypt(const string& message) {
    //placeholder
    return message;
}


int main() {
    /*
    A - Thien:
    1. We don't use IP addresses anymore, replace that with port num (int).
    2. Talk about the objects, I want you to *remake* the entire script of user sending 
    those messages to service server, service server decrypts and checks stuff then returns the results.
    */


    UA user("user123", 1612121212);
    //im assuming i received a ticket (string) with the same structure above.
    string ticket1 = "12345,192.168.1.1,1710234148,1710234268,a1b2c3d4,serviceA";
    string serviceSessionKey = "a1b2c3d4";

    //assuming there was encrypt logic.
    string UAmessage = encrypt(user.createMessage(serviceSessionKey));

    //there is still error while i was testing those function but i didn't have enough time to fix it before meeting.

    user.display();

    user.setUserID("user456");
    user.setTimestamp(1623232323);




    user.display();

    return 0;
}
