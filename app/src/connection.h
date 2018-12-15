#ifndef COFFEE_CHAT_CONNECTION_H_
#define COFFEE_CHAT_CONNECTION_H_

#include <netinet/in.h>
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
  sockaddr MakeAddress(int, char* = nullptr);
  void SendPackage(int, const std::string&);
  std::string RecvPackage(int);
  std::queue<std::string> ParsePackage(std::string&);
  int Epoll(epoll_event*, int, std::map<int, User>* = nullptr);
  void CloseConnection(int);
  void Stop(std::map<int, User>* = nullptr);

  static int default_port_;
  int socket_;
  int signal_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_CONNECTION_H_
