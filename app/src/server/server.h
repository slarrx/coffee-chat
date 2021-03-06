#ifndef COFFEE_CHAT_SERVER_H_
#define COFFEE_CHAT_SERVER_H_

#include <map>
#include <set>
#include <handler.h>

#include "connection.h"
#include "handler.h"
#include "user.h"

namespace coffee_chat {

class Server : public Connection {
 public:
  explicit Server(int = default_port_);
  void Run() override;
 private:
  static void InputHandling(int);
  static void UserProcessing(Server*);
  void AcceptConnection();
  int AddUser(int);
  void RemoveUser(int);
  void ProcessPackages(int);

  int id_counter_;
  std::set<int> free_ids_;
  std::map<int, User> users_;
  Handler handler_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_SERVER_H_
