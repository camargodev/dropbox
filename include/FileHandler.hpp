#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <dirent.h>
#include <vector> 
#include "FileWrapper.hpp"

class FileHandler {
public:
    WrappedFile getFileByFilename(char* filename);
    void deleteFile(char* filename);
    void createFile(char* filename, char* content);
    int getFileSize(const char* filename);
    vector<FileForListing> getFilesInDir(char* dirName);    
    void printFileList(vector<FileForListing> fileList);
    char* getLocalDirectoryName();
    char* getServerDirectoryNameForUser(char* username);

private:
    string path;
    const int FILE_TYPE = 8;
    string DEFAULT_DIR = "sync_dir"; 

    bool isFilenameValid(char* archiveName);
    bool isFile(int archiveType);
};
#endif