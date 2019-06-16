#include "../include/ClientSocketWrapper.hpp"
#include "math.h"

int calculateNumberOfPayloads(const char* filename);

bool ClientSocketWrapper :: setServer(string hostname, int port) {
    this->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    hostent* host = gethostbyname(hostname.c_str());
    if (host != NULL) {
        this->serverAddress = this->buildAddress(*((struct in_addr *)host->h_addr), port);
        return true;
    }
    return false;
}

bool ClientSocketWrapper :: connectToServer() {
    int result = connect(this->socketDescriptor,(struct sockaddr *) &this->serverAddress, sizeof(this->serverAddress));
    return result >= 0;
}

sockaddr_in ClientSocketWrapper :: buildAddress(in_addr hostname, int port) {
    struct sockaddr_in address = buildDefaultAddress(port);
	address.sin_addr = hostname;
    return address;
}

Packet* ClientSocketWrapper :: receivePacketFromServer() {
    return receivePacket(this->socketDescriptor);
}

bool ClientSocketWrapper :: sendPacketToServer(Packet* packet) {
    return sendPacket(this->socketDescriptor, packet);
}

bool ClientSocketWrapper :: identifyUsername(char* username) {
    Packet* packet = new Packet;
    packet->command = IDENTIFICATION;
    strcpy(packet->payload, username);
    packet->payloadSize = sizeof(username);
    strcpy(packet->filename, "");
    packet->currentPartIndex = 1;
    packet->numberOfParts = 1;
    strcpy(packet->ip, this->addressGetter.getIP());
    return sendPacketToServer(packet);
}

bool ClientSocketWrapper :: disconnectFromServer() {
    Packet packet(DISCONNECT);
    bool sendResponse = sendPacketToServer(&packet);
    closeSocket();
    return sendResponse;
}

bool ClientSocketWrapper :: uploadFileToServer(WrappedFile file) {
    if (!file.isFound) {
        printf("Could not find file %s\n", file.filename);
        return false;
    }

    if (!sendFile(UPLOAD_FILE, this->socketDescriptor, file)) {
        printf("Error sending file %s\n", file.filename);
        return false;
    }
    return true;
}

bool ClientSocketWrapper :: askToDownloadFile(char* filename) {
    Packet packet(DOWNLOAD_REQUISITION, sizeof(filename), filename);
    return sendPacketToServer(&packet);
}

bool ClientSocketWrapper :: identifyAsMirror(int port) {
    Packet packet(MIRROR);
    strcpy(packet.ip, this->addressGetter.getIP());
    packet.port = port;
    return sendPacketToServer(&packet);
}

bool ClientSocketWrapper :: identifyAsNewServer(int port) {
    Packet packet;
    strcpy(packet.ip, this->addressGetter.getIP());
    packet.port = port;
    return sendPacketToServer(&packet);
}

bool ClientSocketWrapper :: getFileFromSyncDir(char* filename) {
    Packet packet(ASK_FOR_SYNC_FILE, sizeof(filename), filename);
    return sendPacketToServer(&packet);
}

bool ClientSocketWrapper :: askForFileList() {
    Packet packet(LIST_REQUISITION);
    return sendPacketToServer(&packet);
}

bool ClientSocketWrapper :: deleteFile(char* filename) {
    Packet packet(DELETE_REQUISITION, filename);
    return sendPacketToServer(&packet);
}

void ClientSocketWrapper :: getSyncDir() {
    Packet packet(GET_SYNC_DIR);
    sendPacketToServer(&packet);
}
