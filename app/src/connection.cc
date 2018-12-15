#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <map>
#include <queue>
#include <stdexcept>

#include "connection.h"
#include "user.h"

namespace coffee_chat {

int Connection::default_port_ = 5020;

int Connection::default_port() { return default_port_; }

void Connection::default_port(int default_port) {
  default_port_ = default_port;
}

Connection::Connection() : socket_(socket(AF_INET, SOCK_STREAM, 0)),
                           signal_(eventfd(0, EFD_NONBLOCK)) {
  if (socket_ < 0) {
    throw std::runtime_error("socket()");
  }
}

std::string Connection::Input() {
  std::string message;
  getline(std::cin, message);
  return message;
}

sockaddr Connection::MakeAddress(int port, char* ip) {
  sockaddr_in address = {0};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(port));
  if (ip == nullptr) {
    address.sin_addr.s_addr = INADDR_ANY;
  } else {
    if (!inet_aton(ip, &address.sin_addr)) {
      errno = EINVAL;
      throw std::invalid_argument("inet_aton()");
    }
  }
  return *(sockaddr*)&address;
}

void Connection::SendPackage(int socket, const std::string& package) {
  send(socket, (package + '\n').c_str(), package.length() + 1, 0);
}

std::string Connection::RecvPackage(int socket) {
  char buffer[kMaxMessageLength] = {0};
  std::string package;
  ssize_t total_char = 0;
  ssize_t recv_char = 0;
  do {
    package += buffer;
    recv_char = recv(socket, buffer, kMaxMessageLength, 0);
    total_char += recv_char;
  } while (recv_char > 0 && total_char < kMaxMessageLength);
  return package;
}

std::queue<std::string> Connection::ParsePackage(std::string& package) {
  std::queue<std::string> packages;
  size_t next = 0;
  size_t prev = 0;
  while ((next = package.find('\n', prev)) != std::string::npos) {
    packages.push(package.substr(prev, next - prev));
    prev = next + 1;
  }
  return packages;
}

int Connection::Epoll(epoll_event* ready_events, int max_ready,
                      std::map<int, User>* users_pointer) {
  int epoll_fd = epoll_create(0xFFF);
  if (epoll_fd < 0) {
    throw std::runtime_error("epoll_create()");
  }

  epoll_event* events = nullptr;
  size_t count_events = 0;
  if (users_pointer != nullptr) {
    auto& users = *users_pointer;
    count_events = users.size() + 2;
    events = new epoll_event[count_events];

    for (auto& user : users) {
      events[--count_events].data.ptr = &user.second;
    }

    count_events = users.size() + 2;
  } else {
    count_events = 2;
    events = new epoll_event[count_events];
  }

  events[0].data.ptr = nullptr;
  User signal_container(signal_, -1);
  events[1].data.ptr = &signal_container;

  for (int i = 0; i < count_events; ++i) {
    events[i].events = EPOLLIN;
    int socket = i == 0 ? socket_ : ((User*)events[i].data.ptr)->socket_;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &events[i]) < 0) {
      throw std::runtime_error("epoll_ctl()");
    }
  }

  int count_ready = epoll_wait(epoll_fd, ready_events, max_ready, -1);
  if (count_ready < 0) {
    throw std::runtime_error("epoll_wait()");
  }

  delete[] events;
  return count_ready;
}

void Connection::CloseConnection(int socket) {
  if (shutdown(socket, SHUT_RDWR) < 0) {
    throw std::runtime_error("shutdown()");
  }
  while (true) {
    if (close(socket) == 0) { break; }
    if (errno == EINTR) { continue; }
    throw std::runtime_error("close()");
  }
}

void Connection::Stop(std::map<int, User>* users_pointer) {
  if (users_pointer != nullptr) {
    auto& users = *users_pointer;
    for (auto& user : users) {
      CloseConnection(user.second.socket_);
    }
  }
  CloseConnection(socket_);
}

}  // namespace coffee_chat
