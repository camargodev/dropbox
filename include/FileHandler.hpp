#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <dirent.h>
#include <vector> 
#include "FileWrapper.hpp"

class FileHandler {
public:
    WrappedFile getFileByFilename(char* filename);
    void deleteFile(char* filename);
    void createFile(char* filename, string content, int contentSize);
    void deleteFileOnServer(string username, char* filename);
    void createFileOnServer(string username, char* filename, string content, int contentSize);
    int getFileSize(const char* filename);
    vector<FileForListing> getFilesInDir(char* dirName);
    void printFileList(vector<FileForListing> fileList);
    char* getLocalDirectoryName();
    char* getServerDirectoryNameForUser(string username);
    void createSyncDir(char *username);
    char* getDirName();
    void createServerDir();
    void openClientDir(char *username);
    void setDirName(string username);
    char* downloadFilePath(char *username, char *filename);
    char* getDownloadFilePathForClient(char *username, char *filename);

private:
    string path;
    const int FILE_TYPE = 8;
    string DEFAULT_DIR = "sync_dir";

    bool isFilenameValid(char* archiveName);
    bool isFile(int archiveType);
};
#endif
