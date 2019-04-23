#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ServerSocketWrapper : public SocketWrapper {

    public: 
        ServerSocketWrapper(int port);
        Packet* receivePacketFromClient(SocketDescriptor clientConnectionDescriptor);
        void sendPacketToClient(SocketDescriptor clientConnectionDescriptor, Packet* packet);
        void setNumberOfClients(int numOfClients);
        Connection acceptClientConnection(); 

    private:
        sockaddr_in buildAddress(int port);

};

#endif