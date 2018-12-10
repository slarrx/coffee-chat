#ifndef COFFEE_CHAT_SERVER_H_
#define COFFEE_CHAT_SERVER_H_

#include "connection.h"

namespace coffee_chat {

class Server : public Connection {
 public:
  void Run() override;
 private:
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_SERVER_H_
