#ifndef FILE_WRAPPER_HPP
#define FILE_WRAPPER_HPP

#include <fstream>
#include <string.h>

using namespace std;

// using File = ifstream;
// @Cristiano: need to update to real time type
using Time = int;

const int FILENAME_SIZE = 80;

struct WrappedFile {
    char filename[FILENAME_SIZE];
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