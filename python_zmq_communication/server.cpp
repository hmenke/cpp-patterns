#include "communication.h"
#include "server.h"

#include <chrono>
#include <iostream>
#include <thread>

void perform(int nseconds, std::string const& endpoint)
{
  std::cout << "Welcome to perform()" << std::endl;
  //  Prepare our context and socket
  std::cout << "Create context" << std::endl;
  zmq::context context;
  std::cout << "Create socket" << std::endl;
  zmq::socket publisher = context.socket(ZMQ_PUB);
  std::cout << "Connect socket to endpoint" << std::endl;
  publisher.connect(endpoint.c_str());

  std::cout << "Enter the waiting loop" << std::endl;
  for (int i = 0; i < 5; ++i)
  {
    std::cout << "Waiting for " << nseconds << " s" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds{nseconds});

    std::cout << "Sending loop index" << std::endl;
    publisher.send("%1d", i);
  }

  std::cout << "Sending end signal" << std::endl;
  publisher.send(">>END" );
  std::cout << "Bye... :-)" << std::endl;
}
