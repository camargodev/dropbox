#include "../include/FileHandler.hpp"


WrappedFile FileHandler :: getFileByFilename(char* filename) {
    WrappedFile wrappedFile(filename);
    wrappedFile.file = fopen(filename, "r");
    wrappedFile.foundFile = wrappedFile.file != NULL;
    return wrappedFile;
}

