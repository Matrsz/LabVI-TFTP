#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include "connections.h"

struct Client {
    uint32_t address;
    uint16_t port;
    int fd_file;
    uint16_t block_num;

    Client() : address(0), port(0), fd_file(-1), block_num(0) {}

    Client(uint32_t address, uint16_t port, int fd_file, uint16_t block_num) {
        this->address = address;
        this->port = port;
        this->fd_file = fd_file;
        this->block_num = block_num;
    }
};

// A hash function used to hash a pair of any kind
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        if (hash1 != hash2) {
            return hash1 ^ hash2;             
        }
        return hash1;
    }
};

void printClient(const Client& user) {
    std::cout << "User Information:" << std::endl;
    std::cout << "IP Address: " << (user.address >> 24) << "." << ((user.address >> 16) & 0xFF) << "."
              << ((user.address >> 8) & 0xFF) << "." << (user.address & 0xFF) << std::endl;
    std::cout << "Port: " << user.port << std::endl;
    std::cout << "File Descriptor: " << user.fd_file << std::endl;
    std::cout << "Block Number: " << user.block_num << std::endl;
    return;
}

void printActiveClients(const std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients) {
    std::cout << "Contents of the unordered_map : \n";
    for (auto p : activeClients) {
        std::cout << "[" << (p.first).first << ", "
             << (p.first).second << "] ==> \n";
        printClient(p.second);
    }
    return;
}

bool hasClient(const std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients, uint32_t address, uint16_t port) {
    std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
    return activeClients.find(key) != activeClients.end();
}

void addClient(std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> &activeClients, uint32_t address, uint16_t port) {
    if (!hasClient(activeClients, address, port)) {
        std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
        activeClients[key] = Client(address, port, 0, 0);
    }
    return;
}

void removeClient(std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> &activeClients, uint32_t address, uint16_t port) {
    std::pair<uint32_t, uint16_t> key = std::make_pair(address, port);
    activeClients.erase(key);
    return;
}


void clientInfo(const struct sockaddr_in& clientAddr, uint32_t &address, uint16_t &port) {
    address = ntohs(clientAddr.sin_addr.s_addr);
    port = ntohs(clientAddr.sin_port);
    return;
}