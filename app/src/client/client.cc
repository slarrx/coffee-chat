#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "client.h"

namespace coffee_chat {

Client::Client(char* ip, int port) {
  auto address = MakeAddress(port, ip);
  if (connect(socket_, &address, sizeof(address)) < 0) {
    throw std::runtime_error("connect()");
  }
  fcntl(socket_, F_SETFL, O_NONBLOCK);
}

void Client::Run() {
  epoll_event event = {0};
  std::string package;
  std::queue<std::string> packages;

  std::thread input_thread(InputHandling, this);
  input_thread.detach();

  int exit_flag = false;
  while (!exit_flag) {
    Epoll(&event, 1);
    package = RecvPackage(socket_);
    if (!package.empty()) {
      packages = ParsePackage(package);
      while (!packages.empty()) {
        std::cout << "-> ";
        std::cout << packages.front() << std::endl;
        packages.pop();
      }
    } else {
      Stop();
      exit_flag = true;
    }
  }
}

void Client::InputHandling(Client* client) {
  while (true) {
    std::string command = Input();
    client->SendPackage(client->socket_, command);
  }
}

}  // namespace coffee_chat
