#include "../include/ConnectionHandler.hpp"
#include <algorithm>

ConnectionHandler :: ConnectionHandler() {
    sem_init(&connecting, 0, 1);
}

bool ConnectionHandler :: isClientAlreadyConnected(const string& username) {
    for (auto connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            return true;
    return false;
}

void ConnectionHandler :: addSocketToNewClient(const string& username, ClientInfo clientInfo) {
    sem_wait(&connecting);
    ConnectedUser newClient(username);
    newClient.openConnections.push_back(clientInfo);
    connectedClients.push_back(newClient);
    sem_post(&connecting);
}

void ConnectionHandler :: addSocketToExistingClient(const string& username, ClientInfo clientInfo) {
    for (auto&& connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            connectedClient.openConnections.push_back(clientInfo);
}

void ConnectionHandler :: addSocketToClient(const string& username, ClientInfo clientInfo) {
    if (isClientAlreadyConnected(username))
        addSocketToExistingClient(username, clientInfo);
    else
        addSocketToNewClient(username, clientInfo);
}

vector<ClientInfo> ConnectionHandler :: getSocketsByUsername(const string& username) {
    for (auto connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            return connectedClient.openConnections;
    return {};
}

void ConnectionHandler :: removeSocketFromUser(const string& username, int socket) {
    // for (auto&& connectedClient : connectedClients)
    //     if (username.compare(connectedClient.username) == 0)
    //         connectedClient.openConnections.erase(remove(connectedClient.openConnections.begin(), 
    //                 connectedClient.openConnections.end(), socket), connectedClient.openConnections.end());
}

ConnectedUser ConnectionHandler :: getConnectedClientBySocket(int socket) {
    for (auto connectedClient : connectedClients)
        for (auto openConnection : connectedClient.openConnections)
            if (openConnection.socket == socket)
                return connectedClient;
}