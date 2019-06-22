#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <stdlib.h> 
#include <vector>
#include <string>
#include <string.h>
#include <semaphore.h>
#include "SocketWrapper.hpp"
#include "AddressGetter.hpp"

struct ClientInfo {
    SocketDescriptor socket;
    char ip[INET_ADDRSTRLEN];
    int portToConnect;

    ClientInfo() {}

    ClientInfo(SocketDescriptor socket, char ip[INET_ADDRSTRLEN], int portToConnect) {
        this->socket = socket;
        strcpy(this->ip, ip);
        this->portToConnect = portToConnect;
    }
};

struct ConnectedUser {
    string username;
    bool valid = true;
    vector<ClientInfo> openConnections;

    ConnectedUser(const string& username) {
        this->username = username;
    }

    ConnectedUser(bool valid) {
        this->valid = valid;
    }

    ConnectedUser() {}
};

class ConnectionHandler {
public:
    ConnectionHandler();
    void addSocketToClient(const string& username, ClientInfo clientInfo);
    vector<ClientInfo> getSocketsByUsername(const string& username);
    void removeSocketFromUser(const string& username, SocketDescriptor socket);
    ConnectedUser getConnectedClientBySocket(int socket);
    void disconnectSocket(int socket);
    vector<ConnectedUser> getAllConnectedUsers();
private:
    sem_t connecting;
    vector<ConnectedUser> connectedClients;
    bool isClientAlreadyConnected(const string& username);
    void addSocketToNewClient(const string& username, ClientInfo clientInfo);
    void addSocketToExistingClient(const string& username, ClientInfo clientInfo);

};


#endif