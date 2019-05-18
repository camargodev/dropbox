#include "../include/ServerFileHandler.hpp"

// ==========================================================================================================
// Public methods
// ==========================================================================================================
void ServerFileHandler :: createClientDir(const char* username) {
    const char* dirpath = this->getClientDirpath(username);
    mkdir(dirpath, 07777);
}

// Overloaded
void ServerFileHandler :: createFile(const char* username, const char* filename, string content, int size) {
    const char* clientFilepath = this->getFilepath(username, filename);
    ClientFileHandler::createFile(clientFilepath, content, size);
}

WrappedFile ServerFileHandler :: getFile(const char* username, const char* filename) {
    const char* clientFilepath = this->getFilepath(username, filename);
    return ClientFileHandler::getFile(clientFilepath);
}

int ServerFileHandler :: deleteFile(const char* username, const char* filename) {
    const char* clientFilepath = this->getFilepath(username, filename);
    return ClientFileHandler::deleteFile(clientFilepath);
}

int ServerFileHandler :: getFileSize(const char* username, const char* filename) {
    const char* clientFilepath = this->getFilepath(username, filename);
    return ClientFileHandler::getFileSize(clientFilepath);
}

const char* ServerFileHandler :: getFilepath(const char* username, const char* filename) {
    string clientDirpath = this->getClientDirpath(username);
    string strFilename = this->getFilename(filename);
    return (clientDirpath + strFilename).c_str();
}

vector<FileForListing> ServerFileHandler :: getFiles(const char* username) {
    const char* dirname = this->getClientDirpath(username);
    return this->getFilesByDir(dirname);
}

// ==========================================================================================================
// Private methods
// ==========================================================================================================
const char* ServerFileHandler :: getClientDirpath(const char* username) {
    string strUsername = username;
    string dirpath = this->getDirpath();
    return (dirpath + strUsername + "/").c_str();
}