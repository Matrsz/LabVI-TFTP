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

int openWriteFile (const char* filename);
int openReadFile (const char* filename);
int readFromFile(int file_fd, DATAPacket &packet);
int writeToFile(int file_fd, DATAPacket packet, int bytes_recv);
bool isFileOpen(int file_fd);