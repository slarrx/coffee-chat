#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <iostream>
#include <map>
#include <queue>
#include <stdexcept>

#include "connection.h"
#include "user.h"

namespace coffee_chat {

int Connection::default_port_ = 5021;

Connection::Connection() : socket_(socket(AF_INET, SOCK_STREAM, 0)) {
  if (socket_ < 0) {
    throw std::runtime_error("socket()");
  }
  fcntl(socket_, F_SETFL, O_NONBLOCK);
}

int Connection::default_port() { return default_port_; }

void Connection::default_port(int default_port) {
  default_port_ = default_port;
}

std::string Connection::Input() {
  std::string message;
  std::cout << "-> ";
  std::cin >> message;
  return message;
}

int Connection::Epoll(int max_ready_events, epoll_event* ready_events,
                      int signal, std::map<int, User>* users_pointer) {
  int epoll_fd = epoll_create(0xFFF);
  if (epoll_fd < 0) {
    throw std::runtime_error("epoll_create()");
  }

  epoll_event* events = nullptr;
  if (users_pointer != nullptr) {
    auto& users = *users_pointer;
    size_t count_events = users.size() + 2;
    events = new epoll_event[count_events];

    std::for_each(users.begin(), users.end(), [&](std::pair<int, User> user) {
      --count_events;
      events[count_events].data.ptr = &user.second;
      events[count_events].events = EPOLLIN;
      if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, user.second.socket_,
                    &events[count_events]) < 0) {
        throw std::runtime_error("epoll_ctl()");
      }
    });
  } else {
    events = new epoll_event;
  }

  events[0].data.ptr = nullptr;
  events[0].events = EPOLLIN;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_, &events[0]);

  User signal_container(signal, -1);
  events[1].data.ptr = &signal_container;
  events[1].events = EPOLLIN;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, signal, &events[1]);

  int count_ready = epoll_wait(epoll_fd, ready_events, max_ready_events, -1);
  if (count_ready < 0) {
    throw std::runtime_error("epoll_wait()");
  }

  delete[] events;
  return count_ready;
}

void Connection::SendMessage(int socket, std::string message) {
  send(socket, message.c_str(), message.length(), 0);
}

std::string Connection::RecvMessage(int socket) {
  char buf[kMaxMessageLength] = {0};
  std::string message;
  int count_char = 0;
  ssize_t recv_char = 0;
  do {
    recv_char = recv(socket, buf, kMaxMessageLength, 0);
    count_char += recv_char;
    if (recv_char > 0 && count_char < kMaxMessageLength) {
      message += buf;
    } else {
      break;
    }
  } while (true);
  return message;
}

std::queue<std::string> Connection::ParseMessage(std::string& message) {
  std::queue<std::string> packages;
  size_t next = 0;
  size_t prev = 0;
  while ((next = message.find('\n', prev)) != std::string::npos) {
    packages.push(message.substr(prev, next - prev));
    prev = next + 1;
  }
  return packages;
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

bool Connection::Stop(std::map<int, User>* users_ptr) {
  if (users_ptr != nullptr) {
    auto& users = *users_ptr;
    std::for_each(users.begin(), users.end(), [&](std::pair<int, User> user) {
      CloseConnection(user.second.socket_);
    });
  }
  CloseConnection(socket_);
  return true;
}

}  // namespace coffee_chat
