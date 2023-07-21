#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "connections.h"
#include "packets.h"

#define SERVER_IP "127.0.0.1"
#define PORT 1069


int main() {
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Create UDP socket
    int socket_fd = createSocket();

    // Set server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_aton(SERVER_IP, &serverAddr.sin_addr);

    // Print server's IP address and port
    std::cout << "Server IP: " << SERVER_IP << ", Port: " << PORT << std::endl;
        // Print destination address
    char destIP[INET_ADDRSTRLEN];
    std::cout << "Sending message to " << inet_ntoa(serverAddr.sin_addr) << ", Port: " << ntohs(serverAddr.sin_port) << std::endl;
    

    const char* filename = "example.txt";
    const char* mode = "octet";

    RWPacket rwPacket(OP_RRQ, filename, mode);


    // Print destination address
    std::cout << "Sending message to " <<    inet_ntoa(serverAddr.sin_addr) << ", Port: " << ntohs(serverAddr.sin_port) << std::endl;

    int bytesSent = sendMessage(socket_fd, serverAddr, (void*) &rwPacket, sizeof(OP_RRQ)+strlen(filename)+strlen(mode));
    if (bytesSent < 0) {
        perror("sendto failed");
        close(socket_fd);
        return 1;
    }

    // Receive response from the server
    int bytesRead = receiveMessage(socket_fd, serverAddr, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        perror("recvfrom failed");
        close(socket_fd);
        return 1;
    }

    // Print received message
    buffer[bytesRead] = '\0';
    std::cout << "Received response from server: " << buffer << std::endl;

    close(socket_fd);
    return 0;
}
