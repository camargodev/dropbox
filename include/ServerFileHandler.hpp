#ifndef DROPBOX_SERVERFILEHANDLER_HPP
#define DROPBOX_SERVERFILEHANDLER_HPP

#include "ClientFileHandler.hpp"

class ServerFileHandler : public ClientFileHandler {
    public:
        ServerFileHandler();
        void createClientDir(const char *username);
        void configAsBackup(int port);

        // Overloaded from superclass
        void createFile(const char* username, const char* filename, const char* content, int size);
        void appendFile(const char* username, const char* filename, const char* content, int size);
        WrappedFile getFile(const char* username, const char* filename);
        int deleteFile(const char* username, const char* filename);
        int getFileSize(const char* username, const char* filename);
        string getFilepath(const char* username, const char *filename);
        vector<FileForListing> getFiles(const char* username);

    private:
        string getClientDirpath(const char* username);
};

#endif //DROPBOX_SERVERFILEHANDLER_HPP
