#include "../include/PacketHandler.hpp"

bool PacketHandler :: isFileAlreadyBeingReceivedFromClient(int socket, const string& filename) {
    for (auto fileBeingReceived : filesBeingReceived)
        if (filename.compare(fileBeingReceived.filename) == 0
            && socket == fileBeingReceived.socket)
            return true;
    return false;
}

void PacketHandler :: addPacketToNewFile(int socket, const string& filename, Packet* packet) {
    FileBeingReceived newFile(filename, socket);
    newFile.content = packet->payload;
    filesBeingReceived.push_back(newFile);
}

void PacketHandler :: addPacketToExistingFile(int socket, const string& filename, Packet* packet) {
    for (auto&& fileBeingReceived : filesBeingReceived)
        if (filename.compare(fileBeingReceived.filename) == 0
            && socket == fileBeingReceived.socket) {
            fileBeingReceived.content += packet->payload;
        }
}

void PacketHandler :: addPacketToReceivedFile(int socket, const string& filename, Packet* packet) {
    if (isFileAlreadyBeingReceivedFromClient(socket, filename))
        addPacketToExistingFile(socket, filename, packet);
    else
        addPacketToNewFile(socket, filename, packet);
}

string PacketHandler :: getFileContent(int socket, const string& filename) {
    for (auto&& fileBeingReceived : filesBeingReceived)
        if (filename.compare(fileBeingReceived.filename) == 0
            && socket == fileBeingReceived.socket)
            return fileBeingReceived.content;
}

void PacketHandler :: removeFileFromBeingReceivedList(int socket, const string& filename) {
    int index = -1;
    int counter = 0;
    for (auto&& fileBeingReceived : filesBeingReceived)
        if (filename.compare(fileBeingReceived.filename) == 0
            && socket == fileBeingReceived.socket)
            index = counter;
        counter++;
    if (index != -1)
        filesBeingReceived.erase(filesBeingReceived.begin() + index);
}

// void printFilesBeingReceived() {
//     printf("\nBeing received:");
//     for (auto f : filesBeingReceived) {
//         printf("\nSocket = %i", f.socket);
//         printf("\nFilename = %s", f.filename.c_str());
//         printf("\nContent = %s", f.content.c_str());
//     }
// }

void PacketHandler :: handleReceivedPacket(int socket, Packet* packet) {
    switch (packet->command) {
        case UPLOAD_FILE:
            addPacketToReceivedFile(socket, packet->filename, packet);
            if (packet->currentPartIndex == packet->numberOfParts) {
                string content = getFileContent(socket, packet->filename);
                printf("I finished Receiving file %s with payload %s\n", packet->filename, content.c_str());
                removeFileFromBeingReceivedList(socket, packet->filename);
            }
            break;
        case IDENTIFICATION:
            printf("Client %s connected\n", packet->payload);
            break;
    }
}