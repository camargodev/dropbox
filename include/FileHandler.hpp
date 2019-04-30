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
    vector<string> getFilenamesInDir(char* dirName);    

private:
    string path;
    const int FILE_TYPE = 8;

    bool isFilenameValid(char* archiveName);
    bool isFile(int archiveType);
};
#endif