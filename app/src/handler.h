#ifndef COFFEE_CHAT_HANDLER_H_
#define COFFEE_CHAT_HANDLER_H_

#include <map>
#include <queue>
#include <string>

#include "user.h"

namespace coffee_chat {

class Handler {
 private:
  friend class Server;

  Handler() = default;
  static void Run(std::map<int, User>*);
  void Push(std::string);

  std::queue<std::string> buffer_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_HANDLER_H_
