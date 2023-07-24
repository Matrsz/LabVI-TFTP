#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    const char* SERVER_IP = "127.0.0.1";
    const char* read_filename = "r_example.txt";
    const char* write_filename = "w_example.txt";

    // Fork the child process for writing
    pid_t pid = fork();

    if (pid == 0) {
        // Child process for writing
        execl("./client", "client", SERVER_IP, write_filename, "write", nullptr);
        std::cerr << "Failed to execute client for WRQ." << std::endl;
        return 1;
    } else if (pid > 0) {
        // Parent process for reading
        // Call the client for reading
        execl("./client", "client", SERVER_IP, read_filename, "read", nullptr);
        std::cerr << "Failed to execute client for RRQ." << std::endl;
        return 1;
    } else {
        std::cerr << "Failed to fork process." << std::endl;
        return 1;
    }

    return 0;
}
