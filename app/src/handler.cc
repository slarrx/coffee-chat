#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <fstream>

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

        if (command[0] != '/') {
          handler->RunMsgAll(id, stream);
        } else if (command == "/msg") {
          handler->RunMsg(id, stream);
        } else if (command == "/quit") {
          handler->RunQuit(users[id]);
        } else if (command == "/file") {
          handler->RunFile(id, stream);            
        } else {
          users[id].PushPackage("Unknown command");
        }

        stream.clear();
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

void Handler::RunMsg(int id, std::istringstream& stream) {
  auto& users = *users_;
  int recipient_id;
  stream >> recipient_id;

  if (users_->find(recipient_id) != users_->end()) {
    std::string package;
    std::getline(stream, package);
    package.erase(0, 1);
    users[recipient_id].PushPackage('[' + std::to_string(id) + "]: " + package);
  } else {
    users[id].PushPackage("User is not found");
  }
}

void Handler::RunMsgAll(int id, std::istringstream& stream) {
  auto& users = *users_;

  for (auto& user : users) {
    std::string package = std::move(stream.str());
    package.erase(0, package.find(' ') + 1);
    user.second.PushPackage('[' + std::to_string(id) + "]: " + package);
  }
}

void Handler::RunFile(int id, std::istringstream& stream) {
  auto& users = *users_;
  int recipient_id;
  stream >> recipient_id;

  if (users_->find(recipient_id) != users_->end()) {
    std::string package;
    std::getline(stream, package);
    package.erase(0, 1);
    users[recipient_id].PushPackage("File from [" + std::to_string(id) + "]");
    std::ofstream file("/home/nds/Downloads");
    
    if (!file.is_open()) {
        std::cout << "File not found" << std::endl;
        return;
    }

    file << package;
    file.close();
  } else {
    users[id].PushPackage("User is not found");
  }
}

void Handler::RunQuit(User& user) {
  user.disable_flag_ = true;
}

}  // namespace coffee_chat
