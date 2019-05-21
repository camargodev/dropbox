#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ServerSocketWrapper : public SocketWrapper {

    public: 
        void listenOnPort(int port);
        bool openSocket();
        Packet* receivePacketFromClient(SocketDescriptor clientConnectionDescriptor);
        bool sendPacketToClient(SocketDescriptor clientConnectionDescriptor, Packet* packet);
        void setNumberOfClients(int numOfClients);
        Connection acceptClientConnection(); 
        bool sendSyncFile(SocketDescriptor clientConnectionDescriptor, WrappedFile file);
        bool sendDownloadedFile(SocketDescriptor clientConnectionDescriptor, WrappedFile file);

    private:
        sockaddr_in buildAddress(int port);
        bool sendFileToClient(Command command, SocketDescriptor clientConnectionDescriptor, WrappedFile file);
        int port;

};

#endif