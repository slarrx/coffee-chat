#include <exception>
#include <iostream>
#include <memory>

#include "client.h"

int main(int argc, char* argv[]) {
  if (argc == 1) {
    std::cerr << "Ip address is not specified." << std::endl;
    return 1;
  }

  std::cout << "The client is starting..." << std::endl;

  if (argc > 2) {
    coffee_chat::Client::default_port(
        static_cast<int>(strtol(argv[2], nullptr, 10)));
  }

  try {
    auto client = std::make_shared<coffee_chat::Client>(argv[1]);
    std::cout << "The client is started." << std::endl;
    client->Run();
  } catch (const std::exception& ex) {
    perror(ex.what());
    return -1;
  }

  std::cout << "The client is stopped." << std::endl;

  return 0;
}
