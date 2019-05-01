#include "../include/FileHandler.hpp"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

char dir_name[500];
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

void FileHandler :: deleteFile(char* filename) {
  int status;
  printf("Enter name of a file you wish to delete\n");

  char path_name[400];
  sprintf(path_name, "%s/%s", dir_name, filename);
  status = remove(path_name);

   if (status == 0)
     printf("%s file deleted successfully.\n", filename);
   else
   {
     printf("Unable to delete the file\n");
     perror("Following error occurred");
   }
}

void FileHandler :: createFile(char* filename, string content) {
    // To-Do: @Cristiano
    printf("TO-DO: function to create (file %s)\n", filename);
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
    return (char*) dir_name;
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

void FileHandler :: createSyncDir(char *username) {
    // To-Do: @Cristiano
    //TO-DO: definir nome maximo para o tamanho do diretorio
    char* home = getenv("HOME");
    ::sprintf(dir_name, "%s/sync_dir_%s", home, username);
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dir_name)) != NULL) {
       int i = 0;
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type != DT_DIR) {
                //200 = max file name
                char full_name[200];
                sprintf(full_name, "%s/%s", dir_name, ent->d_name);
                struct stat st;
                if (stat(full_name, &st) != 0) {
                    continue;
                }
                i++;
              }
            }
          closedir (dir);
     } else if (errno == ENOENT) {
        mkdir(dir_name, 07777);
    }
  }
