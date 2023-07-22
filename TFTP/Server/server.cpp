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
#include "users.h"
#include "files.h"

#define PORT 1069

const int blockSize = 512;

void handleMessage(void* buffer, int socket_fd, sockaddr_in clientAddr, std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients){
    uint16_t opcode = getOpcode(buffer);
    std::cout << "Received request from client: " << getOpcode(buffer) << std::endl;
    uint32_t clientIP;
    uint16_t clientPort;
    clientInfo(clientAddr, clientIP, clientPort);
    switch (opcode) {
        case OP_WRQ: {
            char* filename = getFilename(buffer);
            int file_fd = openWriteFile(filename);
            addClient(activeClients, clientIP, clientPort, file_fd);
            std::cout << "Received WRQ of file " << filename << std::endl; 
            printActiveClients(activeClients);
            ACKPacket reply(0);
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply));
            break;
        }
        case OP_RRQ: {
            char* filename = getFilename(buffer);
            int file_fd = openReadFile(filename);
            addClient(activeClients, clientIP, clientPort, file_fd);
            std::cout << "Received RRQ of file " << filename << std::endl; 
            printActiveClients(activeClients);
            DATAPacket reply(1, "");
            int bytesRead = readFromFile(file_fd, reply);
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(OP_DATA)+bytesRead);
            if (bytesRead < blockSize) {
                close(file_fd);
                removeClient(activeClients, clientIP, clientPort);
            }
            break;
        }
        case OP_DATA: {
            Client activeClient = getClient(activeClients, clientIP, clientPort);
            DATAPacket packet = getData(buffer);
            int bytesWritten = writeToFile(activeClient.file_fd, packet);
            ACKPacket reply(activeClient.block_num);
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply));
            if (bytesWritten < blockSize) {
                close(activeClient.file_fd);
                removeClient(activeClients, clientIP, clientPort);
            }
            break;
        }
        case OP_ACK: {
            if (!hasClient(activeClients, clientIP, clientPort)) {
                break;
            }
            Client activeClient = getClient(activeClients, clientIP, clientPort);
            incrementBlockNum(activeClients, clientIP, clientPort);
            DATAPacket reply(activeClient.block_num, "");
            int bytesRead = readFromFile(activeClient.file_fd, reply);
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(OP_DATA)+bytesRead);
            if (bytesRead < blockSize) {
                close(activeClient.file_fd);
                removeClient(activeClients, clientIP, clientPort);
            }
            break;
        }
        default:
            break;
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients;
    
    struct sockaddr_in clientAddr;
    char buffer[1024];

    // Create UDP socket
    int socket_fd = createSocket();
    bindSocket(socket_fd);

    // Receive message from client


    while (true) {
        int bytesRead = receiveMessage(socket_fd, clientAddr, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("recvfrom failed");
            close(socket_fd);
            return 1;
        }
        handleMessage(buffer, socket_fd, clientAddr, activeClients);
    }
    close(socket_fd);
    return 0;
}
