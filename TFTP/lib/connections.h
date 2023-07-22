#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 1069

int createSocket();
void bindSocket(int server_fd);

int receiveMessage(int sockfd, struct sockaddr_in& sourceAddr, char* buffer, int bufferSize);
int sendMessage(int sockfd, const struct sockaddr_in& destAddr, const void* message, int messageSize);
