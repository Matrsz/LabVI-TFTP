#ifndef TFTP_PACKETS_H
#define TFTP_PACKETS_H

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "connections.h"

#define PORT 1069

const uint16_t OP_RRQ = 1;
const uint16_t OP_WRQ = 2;
const uint16_t OP_DATA = 3;
const uint16_t OP_ACK = 4;
const uint16_t OP_ERROR = 5;

struct RWPacket {
    uint16_t opcode;
    char filename[1024];

    RWPacket(uint16_t c_opcode, const char* filename, const char* mode) {
        opcode = htons(c_opcode);
        std::strcpy(this->filename, filename);
        std::strcpy(this->filename + std::strlen(filename) + 1, mode);
    }
};


struct DATAPacket {
    uint16_t opcode;
    uint16_t blockNumber;
    char data[512];

    DATAPacket(uint16_t blockNumber, const char* data) {
        opcode = htons(OP_DATA);
        this->blockNumber = htons(blockNumber);
        std::strcpy(this->data, data);
    }
};

struct ACKPacket {
    uint16_t opcode;
    uint16_t blockNumber;

    ACKPacket(uint16_t blockNumber) {
        opcode = htons(OP_ACK);
        this->blockNumber = htons(blockNumber);
    }
};

struct ERRORPacket {
    uint16_t opcode;
    uint16_t errorCode;
    char errorMsg[512];

    ERRORPacket(uint16_t errorCode, const char* errorMsg) {
        opcode = htons(OP_ERROR);
        this->errorCode = htons(errorCode);
        std::strcpy(this->errorMsg, errorMsg);
    }
};
#endif