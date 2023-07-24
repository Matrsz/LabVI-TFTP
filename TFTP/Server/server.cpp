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

void handleMessage(void* buffer, int bytes_recv, int socket_fd, sockaddr_in clientAddr, ClientsMap &activeClients){
    uint16_t opcode = getOpcode(buffer);
    uint32_t clientIP;
    uint16_t clientPort;
    clientInfo(clientAddr, clientIP, clientPort);
    switch (opcode) {
        case OP_WRQ: {
            char* filename = getFilename(buffer);
            int file_fd = openWriteFile(filename);
            addClient(activeClients, clientIP, clientPort, file_fd);
            std::cout << "==== Received WRQ of file " << filename << " ====" << std::endl; 
            printActiveClients(activeClients, std::cout);
            ACKPacket reply(0);
            std::cout << "Sending ACK of block " << ntohs(reply.hdr.block_num)<< std::endl;
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply));
            break;
        }
        case OP_RRQ: {
            char* filename = getFilename(buffer);
            std::cout << "==== Received RRQ of file " << filename << " ====" << std::endl; 
            int file_fd = openReadFile(filename);
            if (file_fd == -1) {
                ERRORPacket reply(1, "ERROR: File not found.");
                std::cout << "Sending ERROR " << ntohs(reply.hdr.error_code)<< std::endl;
                sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply.hdr)+strlen(reply.errorMsg));
                break;
            }
            addClient(activeClients, clientIP, clientPort, file_fd);
            printActiveClients(activeClients, std::cout);
            DATAPacket reply(1);
            int bytesRead = readFromFile(file_fd, reply);
            std::cout << "Sending DATA block " << ntohs(reply.hdr.block_num)<< std::endl;
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply.hdr) + bytesRead);
            if (bytesRead < blockSize) {
                std::cout << "====== Read request complete ======" << std::endl;
                close(file_fd);
                removeClient(activeClients, clientIP, clientPort);
            }
            break;
        }
        case OP_DATA: {
            const Client *activeClient = getClient(activeClients, clientIP, clientPort);
            if( ! activeClient ) {
                std::cout << "#### problemas\n";
                break;
            }
            DATAPacket packet = getData(buffer);
            std::cout << "Received DATA block " << ntohs(packet.hdr.block_num)<< std::endl;
            int bytesWritten = writeToFile(activeClient->file_fd, packet, bytes_recv-sizeof(packet.hdr));
            ACKPacket reply(activeClient->block_num_ho);
            incrementBlockNum(activeClients, clientIP, clientPort);
            std::cout << "Sending ACK of block " << ntohs(reply.hdr.block_num)<< std::endl;
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply));
            if (bytesWritten < blockSize) {
                std::cout << "====== Write request complete ======" << std::endl;
                close(activeClient->file_fd);
                removeClient(activeClients, clientIP, clientPort);
            }
            break;
        }
        case OP_ACK: {
            ACKPacket packet = getACK(buffer);
            std::cout << "Received ACK of block " << ntohs(packet.hdr.block_num)<< std::endl;
            if (!hasClient(activeClients, clientIP, clientPort)) {
                break;
            }
            const Client *activeClient = getClient(activeClients, clientIP, clientPort);
            if( ! activeClient ) {
                std::cout << "#### problemas 2\n";
                break;
            }
            auto inc_bn_ho = incrementBlockNum(activeClients, clientIP, clientPort);
            DATAPacket reply(inc_bn_ho);
            int bytesRead = readFromFile(activeClient->file_fd, reply);
            std::cout << "Sending DATA block " << ntohs(reply.hdr.block_num)<< std::endl;
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply.hdr)+bytesRead);
            if (bytesRead < blockSize) {
                std::cout << "====== Read request complete ======" << std::endl;
                close(activeClient->file_fd);
                removeClient(activeClients, clientIP, clientPort);
            }
            break;
        }
        case OP_ERROR: {
            ERRORPacket packet = getError(buffer);
            std::cout << "Received error: " << packet.errorMsg << std::endl;
            break;
        }
        default: {
            ERRORPacket reply(4, "Illegal TFTP Operation");
            std::cout << "Sending ERROR " << ntohs(reply.hdr.error_code)<< std::endl;
            sendMessage(socket_fd, clientAddr, (void*) &reply, sizeof(reply.hdr)+strlen(reply.errorMsg));
            break;
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    ClientsMap activeClients;
    
    struct sockaddr_in clientAddr;
    char buffer[1024];

    // Create UDP socket
    int socket_fd = createSocket();
    bindSocket(socket_fd);

    while (true) {
        int bytes_recv = receiveMessage(socket_fd, clientAddr, buffer, sizeof(buffer));
        if (bytes_recv < 0) {
            perror("recvfrom failed");
            close(socket_fd);
            return 1;
        }
        handleMessage(buffer, bytes_recv, socket_fd, clientAddr, activeClients);
    }
    close(socket_fd);
    return 0;
}
