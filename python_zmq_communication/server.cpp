#include "communication.h"
#include "server.h"
#include <iostream>
#include <unistd.h>

void perform(int nseconds)
{
  //  Prepare our context and socket
  zmq::context context;
  zmq::socket publisher = context.socket(ZMQ_PUB);
  publisher.connect("tcp://localhost:5556");

  for (int i = 0; i < 5; ++i)
  {
    sleep(nseconds);

    publisher.send("%1d", i);
  }

  publisher.send(">>END" );
}
