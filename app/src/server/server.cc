#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>
#include <thread>

#include "server.h"

namespace coffee_chat {

Server::Server(int port) : id_counter_(0) {
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
  int signal = eventfd(0, EFD_NONBLOCK);
  std::thread input_thread(InputHanding, signal);
  while (true) {
    epoll_event events[20] = {0};
    int count_events = Epoll(20, events, signal, &users_);

    for (int i = 0; i < count_events; ++i) {
      if (events[i].data.ptr == nullptr) {
        Accept();
      }
    }

  }
}

void Server::InputHanding(int signal) {

}

void Server::Accept() {
  int user_fd = 0;
  while (true) {
    user_fd = accept(socket_, nullptr, nullptr);
    if (user_fd != -1) { break; }
    if (errno == EINTR || errno == ECONNABORTED) { continue; }
    throw std::runtime_error("accept()");
  }
  fcntl(user_fd, F_SETFL, O_NONBLOCK);
  int user_id = AddUser(user_fd);
  SendMessage(user_fd, "Hello, you ID is #" + std::to_string(user_id));
}

int Server::AddUser(int socket) {
  int id = 0;
  if (free_ids_.empty()) {
    id = ++id_counter_;
  } else {
    id = *free_ids_.begin();
    free_ids_.erase(free_ids_.begin());
  }

  users_[id] = User(id, socket);
  return id;
}

}  // namespace coffee_chat
