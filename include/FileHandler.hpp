#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "FileWrapper.hpp"

class FileHandler {
public:
    WrappedFile getFileByFilename(char* filename);
};
#endif