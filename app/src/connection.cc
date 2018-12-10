#include <fcntl.h>
#include <sys/socket.h>
#include <cerrno>
#include <iostream>
#include <stdexcept>

#include "connection.h"

namespace coffee_chat {

int Connection::default_port_ = 5020;

Connection::Connection() : socket_(socket(AF_INET, SOCK_STREAM, 0)) {
  if (socket_ < 0) {
    throw std::runtime_error("socket()");
  }
  fcntl(socket_, F_SETFL, O_NONBLOCK);
}

int Connection::default_port() {
  return default_port_;
}

void Connection::default_port(int default_port) {
  default_port_ = default_port;
}

std::string Connection::Input() {
  std::string message;
  std::cout << "-> ";
  std::cin >> message;
  return message;
}

}  // namespace coffee_chat
