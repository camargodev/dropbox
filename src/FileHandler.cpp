#include "../include/FileHandler.hpp"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

char dir_name[500];
char server_dir[500];
char dir_name_with_file[500];
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

string getFileName(const string& s) {
   char sep = '/';
   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }
   return("");
}

void FileHandler :: deleteFileOnServer(string username, char* filename) {
    char* home = getenv("HOME");
    ::sprintf(dir_name, "%s/server_dir/sync_dir_%s", home, username.c_str());
    this->deleteFile(filename);
}

void FileHandler :: deleteFile(char* filename) {
  int status;
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

void FileHandler :: createFileOnServer(string username, char* filename, string content, int contentSize) {
    char* home = getenv("HOME");
    ::sprintf(dir_name, "%s/server_dir/sync_dir_%s", home, username.c_str());
    this->createFile(filename, content, contentSize);
    printf("CFOS = %s\n", dir_name);
}

void FileHandler :: createFile(char* filename, string content, int contentSize) {
    char *fContent = new char[content.length() + 1];
    strcpy(fContent,content.c_str());

    string simpleFileName = filename;
    if (strstr(filename, "/") != NULL) {
      simpleFileName = getFileName(filename);
    }
    char *fName = new char[simpleFileName.length() + 1];
    strcpy(fName, simpleFileName.c_str());

    FILE * fp;
    char path_name[400];
    sprintf(path_name, "%s/%s", dir_name, fName);
    printf("%s\n", path_name);
    fp = fopen (path_name,"w");
    printf("CONTENT SIZE IS %i\n", contentSize);
    fwrite(&(content), 1, contentSize, fp);
    // fprintf(fp, "%s", fContent);
    fclose (fp);
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
    return (char*) dir_name;
}

char* FileHandler :: getServerDirectoryNameForUser(string username) {
    char *uName = new char[username.length() + 1];
    strcpy(uName,username.c_str());
    char *home = getenv("HOME");
    ::sprintf(server_dir, "%s/server_dir/sync_dir_%s", home, username.c_str());
    return (char*) server_dir;
};

void FileHandler :: createSyncDir(char *username) {
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

  void FileHandler :: createServerDir() {
    char* home = getenv("HOME");
    ::sprintf(dir_name, "%s/server_dir", home);
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

  void FileHandler :: openClientDir(char *username) {
    char* home = getenv("HOME");
    ::sprintf(dir_name, "%s/server_dir/sync_dir_%s", home, username);
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

 char* FileHandler :: getDirName(){
     return dir_name;
 }
 void FileHandler :: setDirName(string username) {
    char *uName = new char[username.length() + 1];
    strcpy(uName,username.c_str());
    char* home = getenv("HOME");
    ::sprintf(dir_name, "%s/server_dir/sync_dir_%s", home, uName);
 }

 char* FileHandler :: downloadFilePath(char *username, char* filename) {
   char* home = getenv("HOME");
   ::sprintf(dir_name_with_file, "%s/server_dir/sync_dir_%s/%s", home, username, filename);
   return (char *) dir_name_with_file;
 }

char* FileHandler :: getDownloadFilePathForClient(char *username, char* filename) {
   char* home = getenv("HOME");
   ::sprintf(dir_name_with_file, "%s/server_dir/sync_dir_%s/%s", home, username, filename);
   return (char *) dir_name_with_file;
 }
