#include "../include/ServerFileHandler.hpp"

// ==========================================================================================================
// Public methods
// ==========================================================================================================
ServerFileHandler :: ServerFileHandler() : ClientFileHandler() {
    this->dirname = "archive/";
}

void ServerFileHandler :: configAsBackup(int port) {
    this->dirname = "backup_archive_" + to_string(port) + "/";
}

void ServerFileHandler :: createClientDir(const char* username) {
    string dirpath = this->getClientDirpath(username);
    mkdir(dirpath.c_str(), 07777);
}

// Overloaded
void ServerFileHandler :: createFile(const char* username, const char* filename, const char* content, int size) {
    string clientFilepath = this->getFilepath(username, filename);
    ClientFileHandler::createFile(clientFilepath.c_str(), content, size);
}

void ServerFileHandler :: appendFile(const char* username, const char* filename, const char* content, int size) {
    string clientFilepath = this->getFilepath(username, filename);
    ClientFileHandler::appendFile(clientFilepath.c_str(), content, size);
}

WrappedFile ServerFileHandler :: getFile(const char* username, const char* filename) {
    string clientFilepath = this->getFilepath(username, filename);
    return ClientFileHandler::getFile(clientFilepath.c_str());
}

int ServerFileHandler :: deleteFile(const char* username, const char* filename) {
    string clientFilepath = this->getFilepath(username, filename);
    return ClientFileHandler::deleteFile(clientFilepath.c_str());
}

int ServerFileHandler :: getFileSize(const char* username, const char* filename) {
    string clientFilepath = this->getFilepath(username, filename);
    return ClientFileHandler::getFileSize(clientFilepath.c_str());
}

string ServerFileHandler :: getFilepath(const char* username, const char* filename) {
    string clientDirpath = this->getClientDirpath(username);
    string strFilename = this->getFilename(filename);
    return clientDirpath + strFilename;
}

vector<FileForListing> ServerFileHandler :: getFiles(const char* username) {
    string dirname = this->getClientDirpath(username);
    return this->getFilesByDir(dirname.c_str());
}

// ==========================================================================================================
// Private methods
// ==========================================================================================================
string ServerFileHandler :: getClientDirpath(const char* username) {
    string strUsername = username;
    string dirpath = this->getDirpath();
    return dirpath + strUsername + "/";
}