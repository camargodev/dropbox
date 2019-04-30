#ifndef PACKET_HPP
#define PACKET_HPP

#include "File.hpp"
#include <string.h>

using namespace std;

const int INITIAL_PART = 0001;

const int PAYLOAD_SIZE = 64;
const int UPLOAD_FILE = 1;
const int IDENTIFICATION = 2;
const int DISCONNECT = 3;
const int SIMPLE_MESSAGE = 4;
const int DOWNLOAD_REQUISITION = 5;
const int DOWNLOADED_FILE = 6;
const int FILE_DOWNLOAD_ERROR = 7;
const int DELETE_REQUISITION = 8;
const int DELETE_ORDER = 9;

struct Packet {

    int command;
    char filename[FILENAME_SIZE];
    int currentPartIndex;
    int numberOfParts;
    int payloadSize;
    char payload[PAYLOAD_SIZE];

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

    Packet(int command,
           int payloadSize,
           char payload[PAYLOAD_SIZE]) {
        this->command = command;
        strcpy(this->filename, "");
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = payloadSize;
        strcpy(this->payload, payload);
    }

    Packet(int command) {
        this->command = command;
        strcpy(this->filename, "");
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = 0;
        strcpy(this->payload, "");
    }

    Packet(int command,
           char filename[FILENAME_SIZE],
           char payload[PAYLOAD_SIZE]) {
        this->command = command;
        strcpy(this->filename, filename);
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = PAYLOAD_SIZE;
        strcpy(this->payload, payload);
    }

    Packet(int command,
           char filename[FILENAME_SIZE]) {
        this->command = command;
        strcpy(this->filename, filename);
        this->currentPartIndex = INITIAL_PART;
        this->numberOfParts = INITIAL_PART;
        this->payloadSize = PAYLOAD_SIZE;
        strcpy(this->payload, "");
    }


};

#endif