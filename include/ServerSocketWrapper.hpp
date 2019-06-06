#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include "SocketWrapper.hpp"
#include "ReplicationHelper.hpp"

using namespace std;

class ServerSocketWrapper : public SocketWrapper {

    public: 
        void listenOnPort(int port);
        bool openSocket();
        Packet* receivePacketFromClient(SocketDescriptor clientConnectionDescriptor);
        Packet* receivePacketFromClient(SocketDescriptor clientConnectionDescriptor, int timeout);
        bool sendPacketToClient(SocketDescriptor clientConnectionDescriptor, Packet* packet);
        void setNumberOfClients(int numOfClients);
        Connection acceptClientConnection(); 
        bool sendSyncFile(SocketDescriptor clientConnectionDescriptor, WrappedFile file);
        bool sendDownloadedFile(SocketDescriptor clientConnectionDescriptor, WrappedFile file);
        bool sendMirror(SocketDescriptor socket, Mirror mirror);

    private:
        sockaddr_in buildAddress(int port);
        bool sendFileToClient(Command command, SocketDescriptor clientConnectionDescriptor, WrappedFile file);
        int port;

};

#endif