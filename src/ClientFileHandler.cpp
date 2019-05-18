#include "../include/ClientFileHandler.hpp"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// ==========================================================================================================
// Public methods
// ==========================================================================================================
ClientFileHandler :: ClientFileHandler() {
    this->path = getenv("HOME");
    this->path = this->path + "/";
    this->dirname = "sync_dir/";
}

void ClientFileHandler :: printFileList(vector<FileForListing> fileList) {
    int longestFilenameSize = getLongestFilenameSize(fileList);

    string title = getFilenameWithSpaces("FILE", longestFilenameSize);
    string filename;

    printf("%s | Modification             | Access                   | Creation                \n", title.c_str());
    for (auto file : fileList) {
        filename = getFilenameWithSpaces(file.filename, longestFilenameSize);
        string m = ctime(&file.modificationTime);
        string a = ctime(&file.accessTime);
        string c = ctime(&file.creationTime);

        m = m.substr(0, m.length() - 1);
        a = a.substr(0, a.length() - 1);
        c = c.substr(0, c.length() - 1);

        printf("%s | %s | %s | %s\n", filename.c_str(), m.c_str(), a.c_str(), c.c_str());
    }
}

string ClientFileHandler :: getFilename(const char* filename) {
    string strFilename = filename;

    if (strstr(filename, "/") != NULL) {
        int size = strFilename.length();

        size_t i = strFilename.rfind('/', size);
        if (i != string::npos)
            return strFilename.substr(i + 1, size - i).c_str();
    }

    return strFilename;
}

string ClientFileHandler :: getDirpath() {
    return this->path + this->dirname;
}

string ClientFileHandler :: getDirname() {
    return this->dirname;
}

void ClientFileHandler :: createDir() {
    string dirpath = this->getDirpath();
    mkdir(dirpath.c_str(), 07777);
}

void ClientFileHandler :: createFile(const char* pathname, string content, int size) {
    this->writeOnFile(pathname, content, size, "w");
}

void ClientFileHandler :: appendFile(const char* pathname, string content, int size) {
    this->writeOnFile(pathname, content, size, "a");
}

WrappedFile ClientFileHandler :: getFile(const char* pathname) {
    ifstream fileStream(pathname);

    string filename = this->getFilename(pathname);
    WrappedFile wrappedFile((char *)filename.c_str());
    wrappedFile.isFound = fileStream.good();
    wrappedFile.filepath = pathname;
    wrappedFile.filesize = this->getFileSize(pathname);

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

string ClientFileHandler :: getFilepath(const char *filename) {
    string dirpath = this->getDirpath();
    string strFilename = this->getFilename(filename);
    return dirpath + strFilename;
}

vector<FileForListing> ClientFileHandler :: getFiles() {
    string dirname = this->getDirpath();
    printf("Getting files...\n"); // print to avoid seg fault (do not know why? crazy stuff)
    return this->getFilesByDir(dirname.c_str());
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
    string strFilename = filename;
    return strFilename.compare(".") != 0 && strFilename.compare("..") != 0;
}

bool ClientFileHandler :: isFile(int filetype) {
    return filetype == FILE_TYPE;
}

void ClientFileHandler :: writeOnFile(const char* pathname, string content, int size, const char* mode) {
    FILE* filePointer;
    filePointer = fopen(pathname, mode);
    fwrite((void *)content.c_str(), 1, size, filePointer);
    fclose(filePointer);
}
