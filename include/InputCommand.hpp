#ifndef COMMAND_HPP
#define COMMAND_HPP

using namespace std;

const int COMMAND_EXIT = 000;
const int COMMAND_UPLOAD = 100;

const int INVALID_COMMAND = -1;

const int COMMAND_SIZE = 128;

union CommandArguments {
    char* fileToUpload;
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