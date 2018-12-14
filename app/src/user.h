#ifndef COFFEE_CHAT_USER_H_
#define COFFEE_CHAT_USER_H_

#include <mutex>
#include <queue>
#include <string>

namespace coffee_chat {

class User {
 public:
  explicit User(int socket = 0, int id = 0)
      : socket_(socket), id_(id), disable_flag_(false) {};

  int socket_;
  int id_;
  bool disable_flag_;
  std::queue<std::string> packages_;
  std::mutex mutex_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_USER_H_
