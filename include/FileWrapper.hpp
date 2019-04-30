#ifndef FILE_WRAPPER_HPP
#define FILE_WRAPPER_HPP

#include <fstream>
#include <string.h>

using namespace std;
using File = FILE;

const int FILENAME_SIZE = 80;

struct WrappedFile {
    char filename[FILENAME_SIZE];
    bool foundFile;
    File* file;

    WrappedFile(char filename[FILENAME_SIZE]) {
        strcpy(this->filename, filename);
    }
};

#endif