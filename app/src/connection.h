#ifndef COFFEE_CHAT_CONNECTION_H_
#define COFFEE_CHAT_CONNECTION_H_

#include <sys/epoll.h>
#include <map>
#include <queue>
#include <string>

#include "user.h"

namespace coffee_chat {

const int kMaxMessageLength = 1024;

class Connection {
 public:
  static int default_port();
  static void default_port(int);
  Connection();
  virtual void Run() = 0;
 protected:
  static std::string Input();
  int Epoll(int, epoll_event*, int, std::map<int, User>* = nullptr);
  void SendMessage(int, std::string);
  std::string RecvMessage(int);
  std::queue<std::string> ParseMessage(std::string&);
  void CloseConnection(int);
  bool Stop(std::map<int, User>* = nullptr);

  static int default_port_;
  int socket_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_CONNECTION_H_
