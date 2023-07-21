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
#include "packets.h"
#include "users.h"

#define PORT 1069

uint16_t getOpcode(const void* buffer) {
    return ntohs(((uint16_t*) buffer)[0]);
}

void printRequest(void* buffer) {
    RWPacket* request = reinterpret_cast<RWPacket*>(buffer);
    std::cout << request->filename << std::endl;
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
    uint32_t clientAddressInfo;
    uint16_t clientPortInfo;
    clientInfo(clientAddr, clientAddressInfo, clientPortInfo);

    addClient(activeClients, clientAddressInfo, clientPortInfo);

    // Print received message
    buffer[bytesRead] = '\0';
    std::cout << "Received request from client: " << getOpcode(buffer) << std::endl;

    printRequest(buffer);

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
