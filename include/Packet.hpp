#ifndef PACKET_HPP
#define PACKET_HPP

#include "File.hpp"

using namespace std;

const int PAYLOAD_SIZE = 64;
const int UPLOAD_FILE = 1;

struct Packet {

    // int type;
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
        this->currentPartIndex = 1;
        this->numberOfParts = 1;
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

    Packet(int payloadSize,
           char payload[PAYLOAD_SIZE]) {
        strcpy(this->filename, "");
        this->currentPartIndex = 1;
        this->numberOfParts = 1;
        this->payloadSize = payloadSize;
        strcpy(this->payload, payload);
    }

};

#endif