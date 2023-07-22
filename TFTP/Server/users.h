#ifndef TFTP_CLIENTS_H
#define TFTP_CLIENTS_H

#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <iostream>

struct Client {
    uint32_t address_ho;
    uint16_t port_ho;
    int file_fd;
    uint16_t block_num_ho;

    // ~Client();
    Client(uint32_t address, uint16_t port, int file_fd, uint16_t block_num);

    void printClient(std::ostream &os);

};

// A hash function used to hash a pair of any kind
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const  {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        if (hash1 != hash2) {
            return hash1 ^ hash2;             
        }
        return hash1;
    }
};

using ClientsMap = std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair>;

void printActiveClients(const ClientsMap &activeClients, std::ostream &os);
bool hasClient(const ClientsMap &activeClients, uint32_t address, uint16_t port);
void addClient(ClientsMap& activeClients, uint32_t address, uint16_t port, int file_fd);
const Client *getClient(ClientsMap &activeClients, uint32_t address, uint16_t port);
void removeClient(ClientsMap& activeClients, uint32_t address, uint16_t port);
uint16_t incrementBlockNum(ClientsMap &activeClients, uint32_t address, uint16_t port);
void clientInfo(const sockaddr_in& clientAddr, uint32_t& address, uint16_t& port);

#endif 
