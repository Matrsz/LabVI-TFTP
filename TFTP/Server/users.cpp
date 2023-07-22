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

using namespace std;

Client::Client(uint32_t address_, uint16_t port_, int file_fd_, uint16_t block_num_)
    : address_ho(address_),
      port_ho(port_),
      file_fd(file_fd_),
      block_num_ho(block_num_)
{
}

// Client::Client()
//     : address_ho(0),
//       port_ho(0),
//       file_fd(0),
//       block_num_ho(0)
// {
// }

// Client::~Client()
// {
//     close(file_fd);
// }

void Client::printClient(ostream &os) {
    os << "User Information:" << std::endl;
    os << "IP Address: " << (address_ho >> 24) << "." << ((address_ho >> 16) & 0xFF) << "."
              << ((address_ho >> 8) & 0xFF) << "." << (address_ho & 0xFF) << std::endl;
    os << "Port: " << port_ho << endl;
    os << "File Descriptor: " << file_fd << endl;
    os << "Block Number: " << block_num_ho << endl;
    return;
}

void printActiveClients(const ClientsMap &activeClients, ostream &os) {
    os << "Contents of the unordered_map : \n";
    for (auto p : activeClients) {
        os << "[" << hex << (p.first).first << ", "
             << dec << (p.first).second << "] ==> \n";
        p.second.printClient(os);
    }
    return;
}

bool hasClient(const ClientsMap &activeClients, uint32_t address, uint16_t port) {
    pair<uint32_t, uint16_t> key = make_pair(address, port);
    return activeClients.find(key) != activeClients.end();
}

void addClient(ClientsMap &activeClients, uint32_t address, uint16_t port, int file_fd) {
    if (!hasClient(activeClients, address, port)) {
        pair<uint32_t, uint16_t> key = make_pair(address, port);
        activeClients.insert(make_pair(key, Client{address, port, file_fd, 1}));
    }
    return;
}

const Client *getClient(ClientsMap &activeClients, uint32_t address, uint16_t port) {
    pair<uint32_t, uint16_t> key = make_pair(address, port);
    auto it = activeClients.find(key);
    if( it == activeClients.end() )
        return nullptr;
    return &it->second;
}

void removeClient(ClientsMap &activeClients, uint32_t address, uint16_t port) {
    pair<uint32_t, uint16_t> key = make_pair(address, port);
    activeClients.erase(key);
    return;
}

uint16_t incrementBlockNum(ClientsMap &activeClients, uint32_t address, uint16_t port) {
    pair<uint32_t, uint16_t> key = make_pair(address, port);
    auto it = activeClients.find(key);
    if( it == activeClients.end() )
        return 0;
    return ++it->second.block_num_ho;
}

void clientInfo(const sockaddr_in& clientAddr, uint32_t &address, uint16_t &port) {
    address = ntohl(clientAddr.sin_addr.s_addr);
    port = ntohs(clientAddr.sin_port);
    return;
}

