#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "SocketWrapper.hpp"

using namespace std;

class ClientSocketWrapper : public SocketWrapper {

    public: 
        Packet* receivePacketFromServer();
        bool sendPacketToServer(Packet* packet);
        bool setServer(string serverHostname, int serverPort);
        bool connectToServer();
        bool uploadFileToServer(WrappedFile file);
        bool identifyUsername(char* username);
        bool disconnectFromServer();
        bool askToDownloadFile(char* filename);
        bool getFileFromSyncDir(char* filename);
        bool deleteFile(char* filename);
        bool askForFileList();
        void getSyncDir();
        bool identifyAsMirror();

    private:
        sockaddr_in buildAddress(in_addr hostname, int port);
        struct sockaddr_in serverAddress;

};

#endif