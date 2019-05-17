#include "../include/FileHandler.hpp"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// ==========================================================================================================
// Public methods
// ==========================================================================================================
FileHandler ::FileHandler() {
    this->path = getenv("HOME");
    this->dirname = "sync_dir";
}

void FileHandler :: printFileList(vector<FileForListing> fileList) {
    int longestFilenameSize = getLongestFilenameSize(fileList);
    string title = getFilenameWithSpaces("FILE", longestFilenameSize);
    string filename;

    printf("%s | M | A | C\n", title.c_str());
    for (auto file : fileList) {
        filename = getFilenameWithSpaces(file.filename, longestFilenameSize);
        printf("%s | %i | %i | %i\n", filename.c_str(), file.modificationTime, file.accessTime, file.creationTime);
    }
}

char* FileHandler :: getFilename(const char* filename) {
    string strFilename = filename;

    if (strstr(filename, "/") != NULL)
        char sep = '/';
        int size = strFilename.length();

        size_t i = strFilename.rfind(sep, size);
        if (i != string::npos)
            return strFilename.substr(i+1, size - i).c_str();

    return strFilename.c_str();
}

char* FileHandler :: getDirpath() {
    return (this->path + this->dirname + "/").c_str();
}

char* FileHandler :: getDirname() {
    return this->dirname.c_str();
}

void FileHandler ::createDir() {
    const char* dirpath = this->getDirpath();
    mkdir(dirpath, 07777);
}

void FileHandler :: createFile(const char* pathname, string content, int size) {
    FILE* filePointer;
    filePointer = fopen(pathname, "w");
    fwrite(&(content), 1, size, filePointer);
    fclose(filePointer);
}

WrappedFile FileHandler :: getFile(const char* pathname) {
    ifstream fileStream(pathname);

    WrappedFile wrappedFile(pathname);
    wrappedFile.isFound = fileStream.good();

    if(wrappedFile.isFound)
        wrappedFile.content.assign(istreambuf_iterator<char>(fileStream), istreambuf_iterator<char>());

    return wrappedFile;
}

int FileHandler :: deleteFile(const char* pathname) {
    return remove(pathname);
}

int FileHandler :: getFileSize(const char* pathname) {
    std::ifstream in(pathname, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

char* FileHandler :: getFilepath(const char *filename) {
    string dirpath = this->getDirpath();
    string strFilename = this->getFilename(filename);
    return (dirpath + strFilename).c_str();
}

vector<FileForListing> FileHandler :: getFiles() {
    char* dirname = this->getDirpath();
    return this->getFilesByDir(dirname);
}

// ==========================================================================================================
// Protected methods
// ==========================================================================================================
vector<FileForListing> FileHandler :: getFilesByDir(const char* dirname) {
    vector <FileForListing> filenames;
    struct stat* stat_info;

    DIR *dir = opendir(dirname);
    struct dirent *currentFile = readdir(directory);

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

        currentFile = readdir(directory);
    }

    closedir(directory);
    return filenames;
}

// ==========================================================================================================
// Private methods
// ==========================================================================================================
string FileHandler :: getFilenameWithSpaces(const char* filename, int filenameTotalSize) {
    int filenameSize = string(filename).size();

    string spaces(filenameTotalSize - filenameSize, ' ');
    string totalFilename = string(filename) + spaces;

    return totalFilename;
}

int FileHandler :: getLongestFilenameSize(vector<FileForListing> fileList) {
    int maxFilenameSize = FILENAME_TITLE_SIZE;
    for (auto fileToList : fileList) {
        int filenameSize = string(fileToList.filename).size();
        if (filenameSize > maxFilenameSize)
            maxFilenameSize = filenameSize;
    }
    return maxFilenameSize;
}

bool FileHandler :: isFilenameValid(const char* filename) {
    string strFilename = string(filename);
    return strFilename.compare(".") != 0 && strFilename.compare("..") != 0;
}

bool FileHandler :: isFile(int filetype) {
    return filetype == FILE_TYPE;
}
