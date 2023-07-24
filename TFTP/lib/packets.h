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
    struct Hdr {
        uint16_t opcode;
        uint16_t block_num;
    } hdr;
    char data[512];

    DATAPacket(uint16_t block_num) {
        hdr.opcode = htons(OP_DATA);
        hdr.block_num = htons(block_num);
    }
};

struct ACKPacket {
    struct Hdr {
        uint16_t opcode;
        uint16_t block_num;
    } hdr;

    ACKPacket(uint16_t block_num_ho) {
        hdr.opcode = htons(OP_ACK);
        hdr.block_num = htons(block_num_ho);
    }
};

struct ERRORPacket {
    struct Hdr {
        uint16_t opcode;
        uint16_t error_code;
    } hdr;
    char errorMsg[512];

    ERRORPacket(uint16_t error_code_ho, const char* errorMsg) {
        hdr.opcode = htons(OP_ERROR);
        hdr.error_code = htons(error_code_ho);
        std::strcpy(this->errorMsg, errorMsg);
    }
};

uint16_t getOpcode(const void* buffer);
char* getFilename(void* buffer);
DATAPacket getData(void* buffer);
ACKPacket getACK(void* buffer);
ERRORPacket getError(void* buffer);

#endif