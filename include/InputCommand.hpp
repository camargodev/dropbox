#ifndef COMMAND_HPP
#define COMMAND_HPP

using namespace std;

const int COMMAND_EXIT = 000;
const int COMMAND_UPLOAD = 100;
const int COMMAND_DOWNLOAD = 200;
const int COMMAND_DELETE = 400;
const int COMMAND_LIST_SERVER = 500;
const int COMMAND_LIST_CLIENT = 600;
const int COMMAND_GET_SỲNC_DIR = 700;

const int INVALID_COMMAND = -1;

const int COMMAND_SIZE = 128;

union CommandArguments {
    char* fileToUpload;
    char* fileToDownload;
    char* fileToDelete;
};

struct Command {
    int commandCode;
    CommandArguments args;

    Command(int code) {
        this->commandCode = code;
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

#endif