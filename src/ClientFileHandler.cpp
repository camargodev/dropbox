#include "../include/ClientFileHandler.hpp"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// ==========================================================================================================
// Public methods
// ==========================================================================================================
ClientFileHandler :: ClientFileHandler() {
    this->path = getenv("HOME");
    this->dirname = "sync_dir";
}

void ClientFileHandler :: printFileList(vector<FileForListing> fileList) {
    int longestFilenameSize = getLongestFilenameSize(fileList);
    string title = getFilenameWithSpaces("FILE", longestFilenameSize);
    string filename;

    printf("%s | M | A | C\n", title.c_str());
    for (auto file : fileList) {
        filename = getFilenameWithSpaces(file.filename, longestFilenameSize);
        printf("%s | %ld | %ld | %ld\n", filename.c_str(), file.modificationTime, file.accessTime, file.creationTime);
    }
}

const char* ClientFileHandler :: getFilename(const char* filename) {
    string strFilename = filename;

    if (strstr(filename, "/") != NULL) {
        int size = strFilename.length();

        size_t i = strFilename.rfind('/', size);
        if (i != string::npos)
            return strFilename.substr(i + 1, size - i).c_str();
    }

    return strFilename.c_str();
}

const char* ClientFileHandler :: getDirpath() {
    return (this->path + this->dirname + "/").c_str();
}

const char* ClientFileHandler :: getDirname() {
    return this->dirname.c_str();
}

void ClientFileHandler :: createDir() {
    const char* dirpath = this->getDirpath();
    mkdir(dirpath, 07777);
}

void ClientFileHandler :: createFile(const char* pathname, string content, int size) {
    FILE* filePointer;
    filePointer = fopen(pathname, "w");
    fwrite(&(content), 1, size, filePointer);
    fclose(filePointer);
}

WrappedFile ClientFileHandler :: getFile(const char* pathname) {
    ifstream fileStream(pathname);

    WrappedFile wrappedFile((char *)pathname);
    wrappedFile.isFound = fileStream.good();

    if(wrappedFile.isFound)
        wrappedFile.content.assign(istreambuf_iterator<char>(fileStream), istreambuf_iterator<char>());

    return wrappedFile;
}

int ClientFileHandler :: deleteFile(const char* pathname) {
    return remove(pathname);
}

int ClientFileHandler :: getFileSize(const char* pathname) {
    std::ifstream in(pathname, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

const char* ClientFileHandler :: getFilepath(const char *filename) {
    string dirpath = this->getDirpath();
    string strFilename = this->getFilename(filename);
    return (dirpath + strFilename).c_str();
}

vector<FileForListing> ClientFileHandler :: getFiles() {
    const char* dirname = this->getDirpath();
    return this->getFilesByDir(dirname);
}

// ==========================================================================================================
// Protected methods
// ==========================================================================================================
vector<FileForListing> ClientFileHandler :: getFilesByDir(const char* dirname) {
    vector <FileForListing> filenames;
    struct stat* stat_info;

    DIR *dir = opendir(dirname);
    struct dirent *currentFile = readdir(dir);

    while (currentFile != NULL) {
        if (this->isFilenameValid(currentFile->d_name) && this->isFile(currentFile->d_type)) {
            string fullName = string(dirname) + string(currentFile->d_name);
            stat(fullName.c_str(), stat_info);

            FileForListing fileForListing(currentFile->d_name);
            fileForListing.modificationTime = stat_info->st_mtime;
            fileForListing.accessTime = stat_info->st_atime;
            fileForListing.creationTime = stat_info->st_ctime;
            filenames.push_back(fileForListing);
        }

        currentFile = readdir(dir);
    }

    closedir(dir);
    return filenames;
}

// ==========================================================================================================
// Private methods
// ==========================================================================================================
string ClientFileHandler :: getFilenameWithSpaces(const char* filename, int filenameTotalSize) {
    int filenameSize = string(filename).size();

    string spaces(filenameTotalSize - filenameSize, ' ');
    string totalFilename = string(filename) + spaces;

    return totalFilename;
}

int ClientFileHandler :: getLongestFilenameSize(vector<FileForListing> fileList) {
    int maxFilenameSize = 0;
    for (auto fileToList : fileList) {
        int filenameSize = string(fileToList.filename).size();
        if (filenameSize > maxFilenameSize)
            maxFilenameSize = filenameSize;
    }
    return maxFilenameSize;
}

bool ClientFileHandler :: isFilenameValid(const char* filename) {
    string strFilename = string(filename);
    return strFilename.compare(".") != 0 && strFilename.compare("..") != 0;
}

bool ClientFileHandler :: isFile(int filetype) {
    return filetype == FILE_TYPE;
}
