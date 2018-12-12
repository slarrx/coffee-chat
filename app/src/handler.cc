#include <unistd.h>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>

#include "handler.h"
#include "user.h"

namespace coffee_chat {

void Handler::Run(Handler* handler, std::map<int, User>* users_ptr) {
  auto& users = *users_ptr;
  while (true) {
    if (!handler->buffer_.empty()) {
      std::queue<std::string> packages;

      handler->buffer_mutex_.lock();
      std::swap(packages, handler->buffer_);
      handler->buffer_mutex_.unlock();

      while (!packages.empty()) {
        int id = 0;
        std::string command;
        std::stringstream stream(packages.front());
        packages.pop();
        stream >> id;
        stream >> command;
        if (command == "msg") {
          int recipient_id;
          stream >> recipient_id;
          std::string message = "[" + std::to_string(id) + "]: " + stream.str();
          users[recipient_id].buffer_.push(message);
        } else {
          users[id].buffer_.push("Unknown command");
        }
      }
    } else {
      usleep(500);
    }
  }
}

void Handler::Push(std::queue<std::string> packages, int id) {
  while (!packages.empty()) {
    buffer_mutex_.lock();
    buffer_.push(std::to_string(id) + " " + packages.front());
    buffer_.pop();
    buffer_mutex_.unlock();
  }
}

}  // namespace coffee_chat
