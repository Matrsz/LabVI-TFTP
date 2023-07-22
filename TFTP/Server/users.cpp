#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include "connections.h"
#include "users.h"

Client::Client(uint32_t address_, uint16_t port_, int file_fd_, uint16_t block_num_)
    : address(address_),
      port(port_),
      file_fd(file_fd_),
      block_num(block_num_)
{
}

Client::Client()
    : address(0),
      port(0),
      file_fd(0),
      block_num(0)
{
}


void Client::printClient(std::ostream &os) {
    os << "User Information:" << std::endl;
    os << "IP Address: " << (address >> 24) << "." << ((address >> 16) & 0xFF) << "."
              << ((address >> 8) & 0xFF) << "." << (address & 0xFF) << std::endl;
    os << "Port: " << port << std::endl;
    os << "File Descriptor: " << file_fd << std::endl;
    os << "Block Number: " << block_num << std::endl;
    return;
}

void printActiveClients(const ClientsMap &activeClients, std::ostream &os) {
    std::cout << "Contents of the unordered_map : \n";
    for (auto p : activeClients) {
        std::cout << "[" << (p.first).first << ", "
             << (p.first).second << "] ==> \n";
        p.second.printClient(os);
    }
    return;
}

bool hasClient(const ClientsMap &activeClients, uint32_t address, uint16_t port) {
    std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
    return activeClients.find(key) != activeClients.end();
}

void addClient(ClientsMap &activeClients, uint32_t address, uint16_t port, int file_fd) {
    if (!hasClient(activeClients, address, port)) {
        std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
        activeClients[key] = Client(address, port, file_fd, 1);
    }
    return;
}

Client getClient(ClientsMap &activeClients, uint32_t address, uint16_t port) {
    std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
    return activeClients[key];
}

void removeClient(ClientsMap &activeClients, uint32_t address, uint16_t port) {
    std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
    activeClients.erase(key);
    return;
}

void incrementBlockNum(ClientsMap &activeClients, uint32_t address, uint16_t port) {
    std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
    activeClients[key].block_num++;
    return;
}

void clientInfo(const sockaddr_in& clientAddr, uint32_t &address, uint16_t &port) {
    address = ntohs(clientAddr.sin_addr.s_addr);
    port = ntohs(clientAddr.sin_port);
    return;
}