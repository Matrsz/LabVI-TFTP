#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include "connections.h"

struct User {
    uint32_t address;
    uint16_t port;
    int fd_file;
    uint16_t block_num;

    User(uint32_t address, uint16_t port, int fd_file, uint16_t block_num) {
        this->address = address;
        this->port = port;
        this->fd_file = fd_file;
        this->block_num = block_num;
    }
};