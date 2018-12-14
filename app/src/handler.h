#ifndef COFFEE_CHAT_HANDLER_H_
#define COFFEE_CHAT_HANDLER_H_

#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>

#include "user.h"

namespace coffee_chat {

class Handler {
 public:
  Handler();
  static void Run(Handler*, std::map<int, User>*);
  void Push(std::queue<std::string>&, int);
 private:
  void PutPackage(User&, std::string);
  void RunMsg(int, std::istringstream&);
  void RunQuit(User&);

  std::map<int, User>* users_;
  std::queue<std::string> packages_;
  std::mutex packages_mutex_;
};

}  // namespace coffee_chat

#endif  // COFFEE_CHAT_HANDLER_H_
