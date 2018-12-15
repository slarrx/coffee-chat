#ifndef COFFEE_CHAT_CLIENT_H_
#define COFFEE_CHAT_CLIENT_H_

#include <string>

#include "connection.h"

namespace coffee_chat {

class Client : public Connection {
 public:
  explicit Client(char*, int = default_port_);
  void Run() override;
 private:
  static void InputHandling(Client*);
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_CLIENT_H_
