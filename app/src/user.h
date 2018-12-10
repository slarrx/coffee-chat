#ifndef COFFEE_CHAT_USER_H_
#define COFFEE_CHAT_USER_H_

namespace coffee_chat {

class User {
 private:
  friend class Server;
  friend class Handler;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_USER_H_
