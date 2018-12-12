#ifndef COFFEE_CHAT_USER_H_
#define COFFEE_CHAT_USER_H_

#include <queue>
#include <string>

namespace coffee_chat {

class User {
 public:
  explicit User(int socket = 0, int id = 0) : socket_(socket), id_(id) {};

  int socket_;
  int id_;
  std::queue<std::string> buffer_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_USER_H_
