#ifndef PACKET_HPP
#define PACKET_HPP

#include "FileWrapper.hpp"
#include "Command.hpp"
#include <string.h>

using namespace std;

const int INITIAL_PART = 0001;
const int PAYLOAD_SIZE = 64;

struct Packet {

    Command command;
    char filename[FILENAME_SIZE];
    int currentPartIndex;
    int numberOfParts;
    int payloadSize;
    char payload[PAYLOAD_SIZE];
    Time modificationTime;
    Time accessTime;
    Time creationTime;

    Packet() {}

    Packet(char payload[PAYLOAD_SIZE]) {
        strcpy(this->payload, payload);
        strcpy(this->filename, "");
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = PAYLOAD_SIZE;
        strcpy(this->payload, payload);
    }

    Packet(char filename[FILENAME_SIZE],
           int currentPartIndex,
           int numberOfParts,
           int payloadSize,
           char payload[PAYLOAD_SIZE]) {
        strcpy(this->filename, filename);
        this->currentPartIndex = currentPartIndex;
        this->numberOfParts = numberOfParts;
        this->payloadSize = payloadSize;
        strcpy(this->payload, payload);
    }

    Packet(Command command,
           int payloadSize,
           char payload[PAYLOAD_SIZE]) {
        this->command = command;
        strcpy(this->filename, "");
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = payloadSize;
        strcpy(this->payload, payload);
    }

    Packet(Command command) {
        this->command = command;
        strcpy(this->filename, "");
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = 0;
        strcpy(this->payload, "");
    }

    Packet(Command command,
           char filename[FILENAME_SIZE],
           char payload[PAYLOAD_SIZE]) {
        this->command = command;
        strcpy(this->filename, filename);
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = PAYLOAD_SIZE;
        strcpy(this->payload, payload);
    }

    Packet(Command command,
           char filename[FILENAME_SIZE]) {
        this->command = command;
        strcpy(this->filename, filename);
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = PAYLOAD_SIZE;
        strcpy(this->payload, "");
    }

    Packet(Command command,
           char filename[FILENAME_SIZE],
           int currentPartIndex,
           int numberOfParts,
           Time modificationTime,
           Time accessTime,
           Time creationTime) {
        this->command = command;
        strcpy(this->filename, filename);
        this->currentPartIndex = currentPartIndex;
        this->numberOfParts = numberOfParts;
        this->modificationTime = modificationTime;
        this->accessTime = accessTime;
        this->creationTime = creationTime;       
    }


};

#endif