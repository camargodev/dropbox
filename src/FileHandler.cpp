#include "../include/FileHandler.hpp"

char FILENAME_TITLE[FILENAME_SIZE] = "FILENAME";
int FILENAME_TITLE_SIZE = string(FILENAME_TITLE).size();

WrappedFile FileHandler :: getFileByFilename(char* filename) {
    ifstream fileStream(filename);
    WrappedFile wrappedFile(filename);
    wrappedFile.isFound = fileStream.good();
    if(wrappedFile.isFound)
        wrappedFile.content.assign((istreambuf_iterator<char>(fileStream)),
                                   (istreambuf_iterator<char>()));
    return wrappedFile;
}

int FileHandler :: getFileSize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

int getLongestFilenameSize(vector<FileForListing> fileList) {
    int maxFilenameSize = FILENAME_TITLE_SIZE;
    for (auto fileToList : fileList) {
        int filenameSize = string(fileToList.filename).size(); 
        if (filenameSize > maxFilenameSize)
            maxFilenameSize = filenameSize;
    }
    return maxFilenameSize;
}

string getFilenameWithSpaces(char* filename, int filenameTotalSize) {
    int filenameSize = string(filename).size();
    string spaces(filenameTotalSize - filenameSize, ' ');
    string totalFilename = string(filename) + spaces;
    return totalFilename;
}

void FileHandler :: printFileList(vector<FileForListing> fileList) {
    int longestFilenameSize = getLongestFilenameSize(fileList);
    string title = getFilenameWithSpaces(FILENAME_TITLE, longestFilenameSize);
    printf("%s | M | A | C\n", title.c_str());
    for (auto file : fileList) {
        string filename = getFilenameWithSpaces(file.filename, longestFilenameSize);
        printf("%s | %i | %i | %i\n", filename.c_str(), file.modificationTime, file.accessTime, file.creationTime);
    }
}

vector<FileForListing> FileHandler :: getFilesInDir(char* dirName) {
    struct dirent *currentFile;
    vector<FileForListing> filenames;
    DIR *directory = opendir(dirName);

    while((currentFile = readdir(directory)) != NULL)
        if (isFilenameValid(currentFile->d_name) && isFile(currentFile->d_type)) {
            FileForListing fileForListing(currentFile->d_name);
            // @Cristiano: Need update to complete with real MAC times
            fileForListing.modificationTime = 0;
            fileForListing.accessTime = 0;
            fileForListing.creationTime = 0;
            filenames.push_back(fileForListing);
        }
    
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

char* FileHandler :: getLocalDirectoryName() {
    // @Cristiano: essa função pega o nome do diretório local do client (só sync_dir)
    // Só descomentar quando o sync_dir estiver sendo criado
    
    // char defaultDir[DEFAULT_DIR.size() + 1];
    // strcpy(defaultDir, DEFAULT_DIR.c_str());
    // defaultDir[DEFAULT_DIR.size()] = '\0';
    // return (char*) defaultDir;
    return (char*) "./input"; 
}

char* FileHandler :: getServerDirectoryNameForUser(string username) {
    // @Cristiano: essa função pega o nome do diretório do client no servidor (sync_dir_username)
    // Só descomentar quando o sync_dir estiver sendo criado
    
    // string fullDirName = DEFAULT_DIR + "_" + username;
    // char defaultDir[fullDirName.size() + 1 ];
    // strcpy(defaultDir, fullDirName.c_str());
    // defaultDir[fullDirName.size()] = '\0';
    // return (char*) defaultDir;
    return (char*) "./";
};


