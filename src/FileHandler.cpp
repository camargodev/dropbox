#include "../include/FileHandler.hpp"


WrappedFile FileHandler :: getFileByFilename(char* filename) {
    WrappedFile wrappedFile(filename);
    wrappedFile.file = fopen(filename, "r");
    wrappedFile.foundFile = wrappedFile.file != NULL;
    return wrappedFile;
}

int FileHandler :: getFileSize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

vector<string> FileHandler :: getFilenamesInDir(char* dirName) {
    struct dirent *currentFile;
    vector<string> filenames;
    DIR *directory = opendir(dirName);

    while((currentFile = readdir(directory)) != NULL)
        if (isFilenameValid(currentFile->d_name) && isFile(currentFile->d_type))
            filenames.push_back(currentFile->d_name);
    
    closedir(directory);

    return filenames;
}

bool FileHandler :: isFilenameValid(char* archiveName) {
    string strArch = string(archiveName);
    return strArch.compare(".") != 0 
        && strArch.compare("..") != 0;
}

bool FileHandler :: isFile(int archiveType) {
    return archiveType == FILE_TYPE;
}


