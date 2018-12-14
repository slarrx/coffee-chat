#include <arpa/inet.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>

#include "client.h"

namespace coffee_chat {

Client::Client(char* ip, int port) {
  auto address = MakeAddress(port, ip);
  if (connect(socket_, &address, sizeof(address)) < 0) {
    throw std::runtime_error("connect()");
  }
}

void Client::Run() {

}

}  // namespace coffee_chat
