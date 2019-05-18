#ifndef FILE_WRAPPER_HPP
#define FILE_WRAPPER_HPP

#include <fstream>
#include <string.h>
#include <sys/stat.h>

using namespace std;

using Time = time_t;
const int FILENAME_SIZE = 80;

struct WrappedFile {
    string filepath;
    char filename[FILENAME_SIZE];
    int filesize;
    bool isFound;
    string content;

    WrappedFile(char filename[FILENAME_SIZE]) {
        strcpy(this->filename, filename);
    }
};

struct FileForListing {
    char filename[FILENAME_SIZE];
    Time modificationTime;
    Time accessTime;
    Time creationTime;

    FileForListing(char filename[FILENAME_SIZE]) {
        strcpy(this->filename, filename);
    }
};

#endif