#ifndef COFFEE_CHAT_HANDLER_H_
#define COFFEE_CHAT_HANDLER_H_

#include <map>
#include <mutex>
#include <queue>
#include <string>

#include "user.h"

namespace coffee_chat {

class Handler {
 private:
  friend class Server;

  Handler() = default;
  static void Run(Handler*, std::map<int, User>*);
  void Push(std::queue<std::string>, int);

  std::queue<std::string> buffer_;
  std::mutex buffer_mutex_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_HANDLER_H_
