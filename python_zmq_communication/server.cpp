#include "communication.h"
#include "server.h"

#include <chrono>
#include <iostream>
#include <thread>

void perform(int nseconds, std::string const& endpoint)
{
  //  Prepare our context and socket
  zmq::context context;
  zmq::socket publisher = context.socket(ZMQ_PUB);
  publisher.connect(endpoint.c_str());

  for (int i = 0; i < 5; ++i)
  {
    std::this_thread::sleep_for(std::chrono::seconds{nseconds});

    publisher.send("%1d", i);
  }

  publisher.send(">>END" );
}
