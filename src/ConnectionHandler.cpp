#include "../include/ConnectionHandler.hpp"
#include <algorithm>

ConnectionHandler :: ConnectionHandler() {
    sem_init(&connecting, 0, 1);
}

bool ConnectionHandler :: isClientAlreadyConnected(const string& username) {
    sem_wait(&connecting);
    bool alreadyConnected = false;
    for (auto connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            alreadyConnected = true;
    sem_post(&connecting);
    return alreadyConnected;
}

void ConnectionHandler :: addSocketToNewClient(const string& username, ClientInfo clientInfo) {
    sem_wait(&connecting);
    ConnectedUser newClient(username);
    newClient.openConnections.push_back(clientInfo);
    connectedClients.push_back(newClient);
    printf("New ip %s:%i connected for client %s\n", clientInfo.ip, clientInfo.portToConnect, username.c_str());
    sem_post(&connecting);
}

void ConnectionHandler :: addSocketToExistingClient(const string& username, ClientInfo clientInfo) {
    sem_wait(&connecting);
    for (auto&& connectedClient : connectedClients) {
        if (username.compare(connectedClient.username) == 0) {
            for (auto&& connection : connectedClient.openConnections) {
                if (strcmp(clientInfo.ip, connection.ip) != 0 || clientInfo.portToConnect != connection.portToConnect) 
                    continue;

                printf("Updating socket %i for user %s (%s:%i)\n", clientInfo.socket, username.c_str(), clientInfo.ip, clientInfo.portToConnect);
                connection.socket = clientInfo.socket;
                sem_post(&connecting);
                return;
            }
            printf("New ip %s:%i connected for client %s\n", clientInfo.ip, clientInfo.portToConnect, username.c_str());
            connectedClient.openConnections.push_back(clientInfo);
        }
    }
    sem_post(&connecting);
}

void ConnectionHandler :: addSocketToClient(const string& username, ClientInfo clientInfo) {
    if (isClientAlreadyConnected(username))
        addSocketToExistingClient(username, clientInfo);
    else
        addSocketToNewClient(username, clientInfo);
}

vector<ClientInfo> ConnectionHandler :: getSocketsByUsername(const string& username) {
    sem_wait(&connecting);
    vector<ClientInfo> connections = {};
    for (auto connectedClient : connectedClients)
        if (username.compare(connectedClient.username) == 0)
            connections = connectedClient.openConnections;
    sem_post(&connecting);
    return connections;
}

void ConnectionHandler :: removeSocketFromUser(const string& username, int socket) {
    // for (auto&& connectedClient : connectedClients)
    //     if (username.compare(connectedClient.username) == 0)
    //         connectedClient.openConnections.erase(remove(connectedClient.openConnections.begin(), 
    //                 connectedClient.openConnections.end(), socket), connectedClient.openConnections.end());
}

ConnectedUser ConnectionHandler :: getConnectedClientBySocket(int socket) {
    sem_wait(&connecting);
    for (auto connectedClient : connectedClients) {
        for (auto openConnection : connectedClient.openConnections) {
            if (openConnection.socket == socket) {
                // printf("Found client %s with socket %i\n", connectedClient.username.c_str(), socket);
                connectedClient.valid = true;
                sem_post(&connecting);
                return connectedClient;
            }
        }
    }
    // printf("Could not find user for socket %i\n", socket);
    sem_post(&connecting);
    return ConnectedUser(false);
}

vector<ConnectedUser> ConnectionHandler :: getAllConnectedUsers() {
    return connectedClients;
}