#ifndef COFFEE_CHAT_SERVER_H_
#define COFFEE_CHAT_SERVER_H_

#include <map>
#include <set>

#include "connection.h"
#include "user.h"

namespace coffee_chat {

class Server : public Connection {
 public:
  explicit Server(int = default_port_);
  void Run() override;
 private:
  static void InputHanding(int);
  void Accept();
  int AddUser(int);

  int id_counter_;
  std::set<int> free_ids_;
  std::map<int, User> users_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_SERVER_H_
