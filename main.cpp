#include <iostream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024

std::string timestamped(const std::string& protocol) {
  // 获取当前时间
  auto now = std::chrono::system_clock::now();
  auto now_time_t = std::chrono::system_clock::to_time_t(now);

  // 转换时间格式
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");

  // 返回带有时间戳和协议标记的字符串
  return ss.str() + " [" + protocol + "] ";
}

void tcp_server(uint16_t port) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  while (true) {
    std::cout << timestamped("TCP") << "Server waiting for connections..." << std::endl;

      if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
          perror("accept");
          continue;
      }

      int read_bytes = read(new_socket, buffer, BUFFER_SIZE);
      if(read_bytes > 0) {
          buffer[read_bytes] = '\0'; // Null-terminate the string
          std::cout << timestamped("TCP") << "Received message from "
                    << inet_ntoa(address.sin_addr) << ": " << buffer << std::endl;
          const char *message = "Hello from TCP server!";
          send(new_socket, message, strlen(message), 0);
      }
      close(new_socket);
  }
}

void udp_server(uint16_t port) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        std::cout << timestamped("UDP") << "Server waiting for messages..." << std::endl;

        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';

        // Print the message with timestamp
        std::cout << timestamped("UDP") << "Received message from "
                  << inet_ntoa(cliaddr.sin_addr) << ": " << buffer << std::endl;

        const char *message = "Hello from UDP server!";
        sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *) &cliaddr, len);
    }
}


int main(int argc, char* argv[]) {
    uint16_t port = std::stoi("8080");

    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }
    std::cout << argv[0] << " listening on port " << port << std::endl;
    std::thread tcp_thread(tcp_server, port);
    std::thread udp_thread(udp_server, port);

    tcp_thread.join();
    udp_thread.join();

    return 0;
}
