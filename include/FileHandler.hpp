#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <dirent.h>
#include <vector> 
#include "FileWrapper.hpp"

class FileHandler {
    public:
        FileHandler();
        void printFileList(vector<FileForListing> fileList);
        char* getFilename(const char* pathname);
        char* getDirpath();
        char* getDirname();
        void createDir();

        void createFile(const char* pathname, string content, int size);
        WrappedFile getFile(const char* pathname);
        int deleteFile(const char* pathname);
        int getFileSize(const char* pathname);
        char* getFilepath(const char *filename);
        vector<FileForListing> getFiles();

    protected:
        vector<FileForListing> FileHandler :: getFilesByDir(const char* dirname);

    private:
        string path;
        string dirname;
        const int FILE_TYPE = 8;

        string getFilenameWithSpaces(const char* filename, int filenameTotalSize);
        int getLongestFilenameSize(vector<FileForListing> fileList);
        bool isFilenameValid(const char* filename);
        bool isFile(int filetype);
};
#endif
