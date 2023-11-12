#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void tcp_client(const std::string& server, uint16_t port) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return;
    }

    std::string hello = "Hello from TCP client!";
    send(sock, hello.c_str(), hello.size(), 0);

    int valread = read(sock, buffer, BUFFER_SIZE);
    std::cout << "Message from server: " << buffer << std::endl;

    close(sock);
}

void udp_client(const std::string& server, uint16_t port, int timeout_sec) {
    int sock;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(server.c_str());

    // 设置接收超时
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        std::cerr << "Setting timeout failed" << std::endl;
        return;
    }

    std::string hello = "Hello from UDP client!";
    sendto(sock, hello.c_str(), hello.size(), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

    socklen_t serv_addr_len = sizeof(serv_addr);
    int n = recvfrom(sock, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &serv_addr_len);
    if (n < 0) {
        std::cerr << "Receive timeout" << std::endl;
    } else {
        buffer[n] = '\0';
        std::cout << "Message from server: " << buffer << std::endl;
    }

    close(sock);
}



int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <protocol> <server> <port>" << std::endl;
        return -1;
    }
    const std::string protocol = argv[1];
    const std::string server = argv[2];
    const uint16_t port = std::stoi(argv[3]);
    if ( protocol == "tcp" ) {
        std::cout << "TCP client..." << std::endl;
        tcp_client(server, port);
    } else if ( protocol == "udp" ) {
        std::cout << "UDP client..." << std::endl;
        udp_client(server, port, 3);
    }

    return 0;
}
