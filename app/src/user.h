#ifndef COFFEE_CHAT_USER_H_
#define COFFEE_CHAT_USER_H_

#include <string>

namespace coffee_chat {

class User {
 public:
  explicit User(int = 0, int = 0);

  int socket_;
  int id_;
  std::string buffer_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_USER_H_
