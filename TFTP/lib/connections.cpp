#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define PORT 1069
#define SERVER_IP "127.0.0.1"

int createSocket() {
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}

void bindSocket(int server_fd) {
    sockaddr_in address = { 0 };
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return;
}


int receiveMessage(int sockfd, struct sockaddr_in& sourceAddr, char* buffer, int bufferSize) {
    socklen_t addrLen = sizeof(sourceAddr);
    int bytesRead = recvfrom(sockfd, buffer, bufferSize, 0, (struct sockaddr*)&sourceAddr, &addrLen);
    std::cout << "Received " << bytesRead << " bytes" << std::endl;
    return bytesRead;
}

int sendMessage(int sockfd, const struct sockaddr_in& destAddr, const void* message, int messageSize) {
    // Print destination address
    std::cout << "Sending message to " << inet_ntoa(destAddr.sin_addr) << ", Port: " << ntohs(destAddr.sin_port) << std::endl;
    
    socklen_t addrLen = sizeof(destAddr);
    int bytesSent = sendto(sockfd, message, messageSize, 0, (struct sockaddr*)&destAddr, addrLen);
    std::cout << "Sent " << bytesSent << " bytes" << std::endl;
    return bytesSent;
}
