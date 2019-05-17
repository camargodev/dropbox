#ifndef DROPBOX_SERVERFILEHANDLER_HPP
#define DROPBOX_SERVERFILEHANDLER_HPP

#include "FileHandler.hpp"

class ServerFileHandler :: FileHandler {
    public:
        void createClientDir(const char *username);

        // Overloaded from superclass
        void createFile(const char* username, const char* filename, string content, int size);
        WrappedFile getFile(const char* username, const char* filename);
        int deleteFile(const char* username, const char* filename);
        int getFileSize(const char* username, const char* filename);
        char* getFilepath(const char* username, const char *filename);
        vector<FileForListing> getFiles(const char* username);

    private:
        char* getClientDirpath(const char* username);
};

#endif //DROPBOX_SERVERFILEHANDLER_HPP
