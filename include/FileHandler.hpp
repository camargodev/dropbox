#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "FileWrapper.hpp"

class FileHandler {
public:
    WrappedFile getFileByFilename(char* filename);
    void deleteFile(char* filename);
    void createFile(char* filename, char* content);
    int getFileSize(const char* filename);

private:
    string path;
};
#endif