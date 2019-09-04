#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <thread>
#include <fstream>
#include <sstream>

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
        std::cout << "\r" << std::right << std::setw(50) << packages.front() << std::endl;
        std::cout << "-> " << std::flush;
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
    std::cout << "-> ";
    std::string input = Input();

    std::string command = input.substr(0, input.find(' '));

    if (command == "/file") {
        std::string path = input.substr(input.find(' ') + 1, input.size());
        input.erase(input.find(' ') + 1, input.size());
        input.append("2 ");
        std::fstream file(path);

        if (!file.is_open()) {
            std::cout << "\rFile not found!" << std::endl << "-> " << std::flush;
            return;
        }

        while (!file.eof()) {
            std::string tmp;
            file >> tmp;
            input.append(tmp);
        }

        file.close();
    }
    client->SendPackage(client->socket_, input);
  }
}

}  // namespace coffee_chat
