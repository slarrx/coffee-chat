#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>

#include "client.h"

namespace coffee_chat {

Client::Client(char* ip, int port) {
  sockaddr_in address = {0};
  address.sin_family = AF_INET;
  address.sin_port = htons(static_cast<uint16_t>(port));
  if (!inet_aton(ip, &address.sin_addr)) {
    errno = EINVAL;
    throw std::invalid_argument("inet_aton()");
  }

  if (connect(socket_, (sockaddr*)&address, sizeof(address)) < 0) {
    throw std::runtime_error("connect()");
  }
}

void Client::Run() {

}

}  // namespace coffee_chat
