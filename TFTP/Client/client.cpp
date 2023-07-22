#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "connections.h"
#include "packets.h"
#include "files.h"

#define SERVER_IP "127.0.0.1"
#define PORT 1069

const int blockSize = 512;

void initializeTransfer(uint16_t opcode, const char* filename, int socket_fd, sockaddr_in serverAddr, int *file_fd, uint16_t &block_num){
    switch (opcode) {
        case OP_WRQ:
            *file_fd = openReadFile(filename);
            block_num = 0;
            break;
        case OP_RRQ: {
            *file_fd = openWriteFile(filename);
            block_num = 1;
            break;
        }
        default:
            break;
    }
    const char* mode = "octet";
    RWPacket message(opcode, filename, mode);
    size_t to_send = sizeof(OP_RRQ)+strlen(filename)+strlen(mode);
    int bytesSent = sendMessage(socket_fd, serverAddr, (void*) &message, to_send);
    if( bytesSent != (int) to_send) {
        // problems...
    }
}

bool handleReply(void* buffer, int socket_fd, sockaddr_in serverAddr, int file_fd, uint16_t &block_num){
    uint16_t opcode = getOpcode(buffer);
    std::cout << "Received reply from server: " << getOpcode(buffer) << std::endl;

    switch (opcode) {
        case OP_DATA: {
            DATAPacket packet = getData(buffer);
            int bytesWritten = writeToFile(file_fd, packet);
            ACKPacket reply(block_num);
            sendMessage(socket_fd, serverAddr, (void*) &reply, sizeof(reply));
            if (bytesWritten < blockSize) {
                close(file_fd);
                return false;
            }
            break;
        }
        case OP_ACK: {
            if (!isFileOpen(file_fd)) {
                return false;
                break;
            }
            block_num += 1;
            DATAPacket reply(block_num, "");
            int bytesRead = readFromFile(file_fd, reply);
            sendMessage(socket_fd, serverAddr, (void*) &reply, sizeof(OP_DATA)+bytesRead);
            if (bytesRead < blockSize) {
                close(file_fd);
            }
            break;
        }
        default:
            break;
    }
    return true;
}

int main() {
    char buffer[1024];

    // Create UDP socket
    int socket_fd = createSocket();

    // Set server address
    // memset(&serverAddr, 0, sizeof(serverAddr));
    sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_aton(SERVER_IP, &serverAddr.sin_addr);

    // Print server's IP address and port
    std::cout << "Server IP: " << SERVER_IP << ", Port: " << PORT << std::endl;
    
    // const char* filename = "example.txt";

    int file_fd = -1;
    uint16_t block_num;

    initializeTransfer(OP_RRQ, "r_example.txt", socket_fd, serverAddr, &file_fd, block_num);

    bool active_transfer = true;
    while(active_transfer) {
        int bytesRead = receiveMessage(socket_fd, serverAddr, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("recvfrom failed");
            close(socket_fd);
            return 1;
        }
        active_transfer = handleReply(buffer, socket_fd, serverAddr, file_fd, block_num);
    }

#if 0
    initializeTransfer(OP_WRQ, "w_example.txt", socket_fd, serverAddr, &file_fd, block_num);

    active_transfer = true;
    while(active_transfer) {
        int bytesRead = receiveMessage(socket_fd, serverAddr, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("recvfrom failed");
            close(socket_fd);
            return 1;
        }
        active_transfer = handleReply(buffer, socket_fd, serverAddr, file_fd, block_num);
    }
#endif
    close(socket_fd);
    return 0;
}
