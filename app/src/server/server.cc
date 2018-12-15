#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <stdexcept>
#include <thread>

#include "handler.h"
#include "server.h"
#include "user.h"

namespace coffee_chat {

Server::Server(int port) : id_counter_(0) {
  fcntl(socket_, F_SETFL, O_NONBLOCK);

  auto address = MakeAddress(port);
  if (bind(socket_, &address, sizeof(address)) < 0) {
    throw std::runtime_error("bind()");
  }

  if (listen(socket_, 5) < 0) {
    throw std::runtime_error("listen()");
  }
}

void Server::Run() {
  std::thread input_thread(InputHandling, signal_);
  std::thread handler_thread(Handler::Run, &handler_, &users_);
  std::thread user_processing_thread(UserProcessing, this);
  input_thread.detach();
  handler_thread.detach();
  user_processing_thread.detach();

  epoll_event events[20] = {0};
  bool exit_flag = false;

  while (!exit_flag) {
    int count_events = Epoll(events, 20, &users_);
    for (int i = 0; i < count_events; ++i) {
      if (events[i].data.ptr == nullptr) {
        AcceptConnection();
      } else {
        auto& user = *(User*)events[i].data.ptr;
        if (user.socket_ == signal_) {
          Stop(&users_);
          exit_flag = true;
        } else {
          ProcessPackages(user.id_);
        }
      }
    }
  }
}

void Server::InputHandling(int signal) {
  bool exit_flag;
  std::string command;
  do {
    std::cout << "-> ";
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

void Server::UserProcessing(Server* server) {
  auto& users = server->users_;
  std::stack<int> disable_users;

  while (true) {
    for (auto& user : users) {
      if (user.second.disable_flag_) {
        disable_users.push(user.second.id_);
      } else {
        while (!user.second.packages_.empty()) {
          server->SendPackage(user.second.socket_, user.second.PopPackage());
        }
      }
    }

    while (!disable_users.empty()) {
      server->RemoveUser(disable_users.top());
      disable_users.pop();
    }
    usleep(200);
  }
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
