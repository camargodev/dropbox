#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <dirent.h>
#include <vector> 
#include "FileWrapper.hpp"

class ClientFileHandler {
    public:
        ClientFileHandler();
        void printFileList(vector<FileForListing> fileList);
        string getFilename(const char* pathname);
        string getDirpath();
        string getDirname();
        void createDir();

        void createFile(const char* pathname, string content, int size);
        WrappedFile getFile(const char* pathname);
        int deleteFile(const char* pathname);
        int getFileSize(const char* pathname);
        string getFilepath(const char *filename);
        vector<FileForListing> getFiles();

    protected:
        string path;
        string dirname;
        vector<FileForListing> getFilesByDir(const char* dirname);

    private:
        const int FILE_TYPE = 8;

        string getFilenameWithSpaces(const char* filename, int filenameTotalSize);
        int getLongestFilenameSize(vector<FileForListing> fileList);
        bool isFilenameValid(const char* filename);
        bool isFile(int filetype);
};
#endif
