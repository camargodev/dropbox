#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ClientSocketWrapper : public SocketWrapper {

    public: 
        Packet* receivePacketFromServer();
        bool sendPacketToServer(Packet* packet);
        bool setServer(string serverHostname, int serverPort);
        bool connectToServer();
        bool uploadFileToServer(char* filename);
        bool identifyUsername(char* username);

    private:
        sockaddr_in buildAddress(in_addr hostname, int port);
        struct sockaddr_in serverAddress;

};

#endif