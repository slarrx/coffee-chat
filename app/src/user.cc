#include "user.h"

namespace coffee_chat {

User::User(int socket, int id) : socket_(socket), id_(id) {}

}  // namespace coffee_chat
