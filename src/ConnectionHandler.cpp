#include "../include/ConnectionHandler.hpp"
#include <algorithm>

bool ConnectionHandler :: isClientAlreadyConnected(const string& username) {
    for (auto connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            return true;
    return false;
}

void ConnectionHandler :: addSocketToNewClient(const string& username, int socket) {
    ConnectedClient newClient(username);
    newClient.openSockets.push_back(socket);
    connectedClients.push_back(newClient);
}

void ConnectionHandler :: addSocketToExistingClient(const string& username, int socket) {
    for (auto&& connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            connectedClient.openSockets.push_back(socket);
}

void ConnectionHandler :: addSocketToClient(const string& username, int socket) {
    if (isClientAlreadyConnected(username))
        addSocketToExistingClient(username, socket);
    else
        addSocketToNewClient(username, socket);
}

vector<int> ConnectionHandler :: getSocketsByUsername(const string& username) {
    for (auto connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            return connectedClient.openSockets;
    return {};
}

void ConnectionHandler :: removeSocketFromUser(const string& username, int socket) {
    for (auto&& connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            connectedClient.openSockets.erase(remove(connectedClient.openSockets.begin(), 
                    connectedClient.openSockets.end(), socket), connectedClient.openSockets.end());
}

ConnectedClient ConnectionHandler :: getConnectedClientBySocket(int socket) {
    for (auto connectedClient : connectedClients)
        for (auto openSocket : connectedClient.openSockets)
            if (openSocket == socket)
                return connectedClient;
}