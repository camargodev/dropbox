#include "../include/InputHandler.hpp"

int InputHandler :: getCommandCode(char* commandName) {
    string strCommand = string(commandName);
    if (strCommand.compare("upload") == 0)
        return INPUT_UPLOAD;
    if (strCommand.compare("exit") == 0)
        return INPUT_EXIT;
    if (strCommand.compare("download") == 0)
        return INPUT_DOWNLOAD;
    if (strCommand.compare("delete") == 0)
        return INPUT_DELETE;
    if (strCommand.compare("list_client") == 0)
        return INPUT_LIST_CLIENT;
    if (strCommand.compare("list_server") == 0)
        return INPUT_LIST_SERVER;
    if (strCommand.compare("get_sync_dir") == 0)
        return INPUT_GET_SYNC_DIR;
    return INVALID_INPUT;
}

char* InputHandler :: getFilenameOnInput() {
    char* filename = strtok(NULL, "\0");
    int filenameSize = strlen(filename);
    if (filename[filenameSize - 1] == '\n')
        filename[filenameSize - 1] = 0;
    return filename;
}

Input InputHandler :: processCommand(char userInput[INPUT_SIZE]) {
    char *inputOperation = strtok(userInput, " ");
    int inputOperationSize = strlen(inputOperation);
    if (inputOperation[inputOperationSize - 1] == '\n')
        inputOperation[inputOperationSize - 1] = 0;
    int inputCode = getCommandCode(inputOperation);
    Input input(inputCode);
    switch (inputCode) {
        case INPUT_UPLOAD:
            input.args.fileToUpload = getFilenameOnInput();
            break;
        case INPUT_DOWNLOAD:
            input.args.fileToDownload = getFilenameOnInput();
            break;
        case INPUT_DELETE:
            input.args.fileToDelete = getFilenameOnInput();
            break;
        case INPUT_LIST_CLIENT:
            input.args.directory = this->clientSyncDirName;
            break;
    }
    return input;
}

void InputHandler :: setClientSyncDirName(char* syncDirName) {
    this->clientSyncDirName = syncDirName;
}