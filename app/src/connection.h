#ifndef COFFEE_CHAT_CONNECTION_H_
#define COFFEE_CHAT_CONNECTION_H_

#include <string>

namespace coffee_chat {

class Connection {
 public:
  static int default_port();
  static void default_port(int);
  Connection();
  virtual void Run() = 0;
 protected:
  std::string Input();

  static int default_port_;
  int socket_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_CONNECTION_H_
