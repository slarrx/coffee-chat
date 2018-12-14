#include <unistd.h>
#include <algorithm>
#include <map>
#include <queue>
#include <sstream>
#include <string>

#include "handler.h"
#include "user.h"

namespace coffee_chat {

Handler::Handler() : users_(nullptr) {}

void Handler::Run(Handler* handler, std::map<int, User>* users_pointer) {
  handler->users_ = users_pointer;
  auto& users = *users_pointer;
  std::queue<std::string> packages;
  std::istringstream stream;
  std::string command;

  while (true) {
    if (!handler->packages_.empty()) {
      handler->packages_mutex_.lock();
      std::swap(packages, handler->packages_);
      handler->packages_mutex_.unlock();

      while (!packages.empty()) {
        stream.str(packages.front());
        packages.pop();

        int id;
        stream >> id;
        stream >> command;

        if (command == "msg") {
          handler->RunMsg(id, stream);
        } else if (command == "quit") {
          handler->RunQuit(users[id]);
        } else {
          handler->PutPackage(users[id], "Unknown command");
        }
      }
    } else {
      usleep(200);
    }
  }
}

void Handler::Push(std::queue<std::string>& packages, int id) {
  std::string package;
  while (!packages.empty()) {
    package = std::to_string(id) + ' ' + packages.front();
    packages.pop();
    packages_mutex_.lock();
    packages_.push(package);
    packages_mutex_.unlock();
  }
}

void Handler::PutPackage(User& user, std::string package) {
  user.mutex_.lock();
  user.packages_.push(package);
  user.mutex_.unlock();
}

void Handler::RunMsg(int id, std::istringstream& stream) {
  auto& users = *users_;
  int recipient_id;
  stream >> recipient_id;

  if (users_->find(recipient_id) != users_->end()) {
    std::string package;
    std::getline(stream, package);
    PutPackage(users[recipient_id], package);
  } else {
    PutPackage(users[id], "User is not found");
  }
}

void Handler::RunQuit(User& user) {
  user.disable_flag_ = true;
}

}  // namespace coffee_chat
