#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <unordered_map>
#include "connections.h"
#include <fcntl.h>
#include "packets.h"

#define PORT 1069

const int blockSize = 512;

int openWriteFile (const char* filename) {
    int file_fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (file_fd == -1) {
        // Failed to open the file
        std::cerr << "Error opening file: " << filename << std::endl;
        return -1;
    }
    return file_fd;
}

int openReadFile (const char* filename) {
    int file_fd = open(filename, O_RDONLY);

    if (file_fd == -1) {
        // Failed to open the file
        std::cerr << "Error opening file: " << filename << std::endl;
    }
    return file_fd;
}

int readFromFile(int file_fd, DATAPacket &packet) {
    int offset = blockSize*(ntohs(packet.hdr.block_num)-1);
    ssize_t bytesRead = pread(file_fd, packet.data, sizeof(packet.data), offset);
    std::cout << "Read " << sizeof(packet.data) << " bytes\n";

    if (bytesRead == -1) {
        // Error while reading
        std::cerr << "Error reading file at offset " << offset << std::endl;
        close(file_fd);
    }    
    return bytesRead;
}

int writeToFile(int file_fd, DATAPacket packet) {
    int offset = blockSize*(ntohs(packet.hdr.block_num)-1);
    std::cout << "Writing " << strlen(packet.data) << " bytes: " << packet.data << std::endl;
    ssize_t bytesWritten = pwrite(file_fd, packet.data, strlen(packet.data), offset);

    if (bytesWritten == -1) {
        // Error while writing
        std::cerr << "Error writing file at offset " << offset << std::endl;
        close(file_fd);
    }
    return bytesWritten;
}

bool isFileOpen(int file_fd) {
    int flags = fcntl(file_fd, F_GETFD);

    if (flags == -1) {
        std::cerr << "Error checking file descriptor" << std::endl;
        return false;
    }
    bool isOpen = (flags & FD_CLOEXEC) == 0;
    return isOpen;
}