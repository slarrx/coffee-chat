#ifndef COFFEE_CHAT_CONNECTION_H_
#define COFFEE_CHAT_CONNECTION_H_

namespace coffee_chat {

const int kDefaultPort = 5020;

class Connection {
 public:
  Connection();
  virtual void Run() = 0;
 protected:
  int socket_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_CONNECTION_H_
