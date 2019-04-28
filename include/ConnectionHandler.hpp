#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <stdlib.h> 
#include <vector>
#include <string>
#include <string.h>
#include "SocketWrapper.hpp"

struct ConnectedClient {
    string username;
    vector<SocketDescriptor> openSockets;

    ConnectedClient(const string& username) {
        this->username = username;
    }
};

class ConnectionHandler {
public:
    void addSocketToClient(const string& username, SocketDescriptor socket);
    vector<SocketDescriptor> getSocketsByUsername(const string& username);
    void removeSocketFromUser(const string& username, SocketDescriptor socket);
private:
    vector<ConnectedClient> connectedClients;
    bool isClientAlreadyConnected(const string& username);
    void addSocketToNewClient(const string& username, SocketDescriptor socket);
    void addSocketToExistingClient(const string& username, SocketDescriptor socket);

};


#endif