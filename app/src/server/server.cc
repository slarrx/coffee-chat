#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <cerrno>
#include <iostream>
#include <map>
#include <queue>
#include <stdexcept>
#include <thread>

#include "handler.h"
#include "server.h"
#include "user.h"

namespace coffee_chat {

Server::Server(int port) : id_counter_(0) {
  auto address = MakeAddress(port);
  if (bind(socket_, &address, sizeof(address)) < 0) {
    throw std::runtime_error("bind()");
  }

  if (listen(socket_, 5) < 0) {
    throw std::runtime_error("listen()");
  }
}

void Server::Run() {

}

void Server::InputHandling(int signal) {
  bool exit_flag;
  std::string command;
  do {
    command = Input();
    if (command == "stop") {
      eventfd_write(signal, 1);
      exit_flag = true;
    } else {
      std::cout << "Unknown command" << std::endl;
      exit_flag = false;
    }
  } while (!exit_flag);
}

void Server::AcceptConnection() {
  int socket = 0;
  while (true) {
    socket = accept(socket_, nullptr, nullptr);
    if (socket != -1) { break; }
    if (errno == EINTR || errno == ECONNABORTED) { continue; }
    throw std::runtime_error("accept()");
  }
  fcntl(socket, F_SETFL, O_NONBLOCK);
  int id = AddUser(socket);
  std::string message = "Hello, your ID is #" + std::to_string(id);
  SendPackage(socket, message);
}

int Server::AddUser(int socket) {
  int id = 0;
  if (free_ids_.empty()) {
    id = ++id_counter_;
  } else {
    id = *free_ids_.begin();
    free_ids_.erase(free_ids_.begin());
  }
  users_[id].id_ = id;
  users_[id].socket_ = socket;
  return id;
}

void coffee_chat::Server::RemoveUser(int id) {
  CloseConnection(users_[id].socket_);
  users_.erase(id);
  free_ids_.insert(id);
}

void Server::ProcessPackages(int id) {
  std::string package = RecvPackage(users_[id].socket_);
  if (package.length() == 0) {
    RemoveUser(id);
  } else {
    auto packages = ParsePackage(package);
    handler_.Push(packages, id);
  }
}

}  // namespace coffee_chat
