#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include <vector>
#include <string>
#include "Packet.hpp"

struct FileBeingReceived {
    string filename;
    string content = "";
    unsigned long long contentSize = 0;
    int socket;

    FileBeingReceived(const string& filename, int socket) {
        this->filename = filename;
        this->socket = socket;
    }
};

class PacketHandler {
public:
    void addPacketToReceivedFile(int socket, const string& filename, Packet* packet);
    string getFileContent(int socket, const string& filename);
    int getFileContentSize(int socket, const string& filename);
    void removeFileFromBeingReceivedList(int socket, const string& filename);
    // void handleReceivedPacket(int socket, Packet* packet);

private:
    vector<FileBeingReceived> filesBeingReceived;
    bool isFileAlreadyBeingReceivedFromClient(int socket, const string& filename);
    void addPacketToNewFile(int socket, const string& filename, Packet* packet);
    void addPacketToExistingFile(int socket, const string& filename, Packet* packet);

};

#endif