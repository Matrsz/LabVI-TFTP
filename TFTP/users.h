#ifndef TFTP_CLIENTS_H
#define TFTP_CLIENTS_H

#include <cstdint>
#include <cstring>
#include <unordered_map>

struct Client {
    uint32_t address;
    uint16_t port;
    int fd_file;
    uint16_t block_num;

    Client();
    Client(uint32_t address, uint16_t port, int fd_file, uint16_t block_num);
};

// A hash function used to hash a pair of any kind
struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const;
};

void printClient(const Client& user);
void printActiveClients(const std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients);
bool hasClient(const std::unordered_map<std::pair<uint32_t,uint16_t>,Client,hash_pair> activeClients, uint32_t address, uint16_t port);
void addClient(std::unordered_map<std::pair<uint32_t, uint16_t>, Client, hash_pair>& activeClients, uint32_t address, uint16_t port);
void removeClient(std::unordered_map<std::pair<uint32_t, uint16_t>, Client, hash_pair>& activeClients, uint32_t address, uint16_t port);
void clientInfo(const struct sockaddr_in& clientAddr, uint32_t& address, uint16_t& port);

#endif 
