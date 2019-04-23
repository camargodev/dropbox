#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ClientSocketWrapper : public SocketWrapper {

    public: 
        ClientSocketWrapper(string serverHostname, int serverPort);
        Packet* receivePacketFromServer();
        void sendPacketToServer(Packet* packet);
        bool foundHostName = false;

    private:
        sockaddr_in buildAddress(in_addr hostname, int port);

};

#endif