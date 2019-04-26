#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ServerSocketWrapper : public SocketWrapper {

    public: 
        ServerSocketWrapper(int port);
        bool openSocket();
        Packet* receivePacketFromClient(SocketDescriptor clientConnectionDescriptor);
        bool sendPacketToClient(SocketDescriptor clientConnectionDescriptor, Packet* packet);
        void setNumberOfClients(int numOfClients);
        Connection acceptClientConnection(); 
        void sendFileToClient(SocketDescriptor clientConnectionDescriptor, File* file);

    private:
        sockaddr_in buildAddress(int port);
        int port;

};

#endif