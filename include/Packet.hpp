#ifndef PACKET_HPP
#define PACKET_HPP

#include "File.hpp"

using namespace std;

const int PAYLOAD_SIZE = 4096;

struct Packet {

    int type;
    int command;
    char filename[FILENAME_SIZE];
    int seqnr;
    int totalSize;
    char payload[PAYLOAD_SIZE];

};

#endif