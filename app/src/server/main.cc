#include <exception>
#include <iostream>
#include <memory>

#include "server.h"

int main(int argc, char* argv[]) {
  std::cout << "The server is starting..." << std::endl;

  if (argc > 1) {
    coffee_chat::Server::default_port(
        static_cast<int>(strtol(argv[1], nullptr, 10)));
  }

  try {
    auto server = std::make_shared<coffee_chat::Server>();
    std::cout << "The server is started." << std::endl;
    server->Run();
  } catch (const std::exception& ex) {
    perror(ex.what());
    return -1;
  }

  std::cout << "The server is stopped." << std::endl;

  return 0;
}
