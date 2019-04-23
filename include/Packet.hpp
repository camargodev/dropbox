#ifndef PACKET_HPP
#define PACKET_HPP

using namespace std;

const int PAYLOAD_SIZE = 4096;

struct Packet {

    int type;
    int command;
    int seqnr;
    int totalSize;
    char payload[PAYLOAD_SIZE];

};

#endif