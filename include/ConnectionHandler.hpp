#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <stdlib.h> 
#include <vector>
#include <string>
#include <string.h>
#include <semaphore.h>
#include "SocketWrapper.hpp"

struct ConnectedClient {
    string username;
    vector<SocketDescriptor> openSockets;

    ConnectedClient(const string& username) {
        this->username = username;
    }

    ConnectedClient() {}
};

class ConnectionHandler {
public:
    ConnectionHandler();
    void addSocketToClient(const string& username, SocketDescriptor socket);
    vector<SocketDescriptor> getSocketsByUsername(const string& username);
    void removeSocketFromUser(const string& username, SocketDescriptor socket);
    ConnectedClient getConnectedClientBySocket(int socket);
    void disconnectSocket(int socket);
private:
    sem_t connecting;
    vector<ConnectedClient> connectedClients;
    bool isClientAlreadyConnected(const string& username);
    void addSocketToNewClient(const string& username, SocketDescriptor socket);
    void addSocketToExistingClient(const string& username, SocketDescriptor socket);

};


#endif