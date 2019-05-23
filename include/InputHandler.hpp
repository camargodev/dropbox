#ifndef INPUT_HPP
#define INPUT_HPP
#include <string>
#include <string.h>

using namespace std;

const int INPUT_EXIT = 000;
const int INPUT_UPLOAD = 100;
const int INPUT_DOWNLOAD = 200;
const int INPUT_DELETE = 400;
const int INPUT_LIST_SERVER = 500;
const int INPUT_LIST_CLIENT = 600;
const int INPUT_GET_SYNC_DIR = 700;

const int INVALID_INPUT = -1;

const int INPUT_SIZE = 128;

union InputArguments {
    char* fileToUpload;
    char* fileToDownload;
    char* fileToDelete;
    char* directory;
};

struct Input {
    int inputCode;
    InputArguments args;

    Input(int code) {
        this->inputCode = code;
    }
};

struct ClientInput{
    char* username;
    char* serverHostname;
    int serverPort;

    ClientInput(char* username,
                char* serverHostname,
                int serverPort) {
        this->username = username;
        this->serverHostname = serverHostname;
        this->serverPort = serverPort;
    }
};

class InputHandler {
public:
    void setClientSyncDirName(char* syncDirName);
    Input processCommand(char userInput[INPUT_SIZE]);
private:
    char* clientSyncDirName;
    int getCommandCode(char* commandName);
    char* getFilenameOnInput();
};

#endif