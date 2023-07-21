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

#define PORT 1069

const int blockSize = 512;

uint16_t getOpcode(const void* buffer) {
    return ntohs(((uint16_t*) buffer)[0]);
}

char* getFilename(void* buffer) {
    RWPacket* request = reinterpret_cast<RWPacket*>(buffer);
    return request->filename;
}

int openWriteFile (char* filename) {
    int file_fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (file_fd == -1) {
        // Failed to open the file
        std::cerr << "Error opening file: " << filename << std::endl;
        return -1;
    }
    return file_fd;
}

int openReadFile (char* filename) {
    int file_fd = open(filename, O_RDONLY);

    if (file_fd == -1) {
        // Failed to open the file
        std::cerr << "Error opening file: " << filename << std::endl;
    }
    return file_fd;
}

int readFromFile(Client client, DATAPacket &packet) {
    ssize_t bytesRead = pread(client.file_fd, packet.data, sizeof(packet.data), blockSize*client.block_num);

    if (bytesRead == -1) {
        // Error while reading
        std::cerr << "Error reading file at offset " << blockSize*client.block_num << std::endl;
        close(client.file_fd);
    }    
    return bytesRead;
}

int writeToFile(Client client, DATAPacket packet) {
    ssize_t bytesWritten = pwrite(client.file_fd, packet.data, strlen(packet.data), blockSize*client.block_num);

    if (bytesWritten == -1) {
        // Error while writing
        std::cerr << "Error writing file at offset " << blockSize*client.block_num << std::endl;
        close(client.file_fd);
    }
    return bytesWritten;
}


void handleMessage(void* buffer, sockaddr_in clientAddr, std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients){
    uint16_t opcode = getOpcode(buffer);
    uint32_t clientIP;
    uint16_t clientPort;
    clientInfo(clientAddr, clientIP, clientPort);
    switch (opcode) {
    case OP_RRQ:
        char* filename = getFilename(buffer);
        int file_fd = openWriteFile(filename);
        addClient(activeClients, clientIP, clientPort, file_fd);
        break;
    case OP_WRQ:
        char* filename = getFilename(buffer);
        int file_fd = openReadFile(filename);
        addClient(activeClients, clientIP, clientPort, file_fd);
        break;
    case OP_DATA:
        Client activeClient = getClient(activeClients, clientIP, clientPort);
        incrementBlockNum(activeClients, clientIP, clientPort);
        break;
    case OP_ACK:
        Client activeClient = getClient(activeClients, clientIP, clientPort);
        incrementBlockNum(activeClients, clientIP, clientPort);
        DATAPacket reply(activeClient.block_num, "");
        readFromFile(activeClient, reply);
        break;
    default:
        break;
    }
}

int main(int argc, char const* argv[]) {
    std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients;
    
    struct sockaddr_in clientAddr;
    char buffer[1024];

    // Create UDP socket
    int socket_fd = createSocket();
    bindSocket(socket_fd);

    // Receive message from client
    int bytesRead = receiveMessage(socket_fd, clientAddr, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("recvfrom failed");
        close(socket_fd);
        return 1;
    }

    // Print client information
    uint32_t clientIP;
    uint16_t clientPort;
    clientInfo(clientAddr, clientIP, clientPort);


    // Print received message
    buffer[bytesRead] = '\0';
    std::cout << "Received request from client: " << getOpcode(buffer) << std::endl;

    // Send response back to the client
    const char* response = "Message received!";
    int bytesSent = sendMessage(socket_fd, clientAddr, (void*) response, strlen(response));
    if (bytesSent < 0) {
        perror("sendto failed");
        close(socket_fd);
        return 1;
    }
    printActiveClients(activeClients);

    close(socket_fd);
    return 0;
}
