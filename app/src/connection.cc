#include <fcntl.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>

#include "connection.h"

namespace coffee_chat {

Connection::Connection() : socket_(socket(AF_INET, SOCK_STREAM, 0)) {
  if (socket_ < 0) {
    throw std::runtime_error("socket()");
  }
  fcntl(socket_, F_SETFL, O_NONBLOCK);
}

}  // namespace coffee_chat
