#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>

#include "server.h"

namespace coffee_chat {

Server::Server(int port) {
  sockaddr_in address = {0};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(port));
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(socket_, (sockaddr*)&address, sizeof(address)) < 0) {
    throw std::runtime_error("bind()");
  }

  if (listen(socket_, 5) < 0) {
    throw std::runtime_error("listen()");
  }
}

void Server::Run() {

}

}  // namespace coffee_chat
