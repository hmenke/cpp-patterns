%module python_zmq_communication

%include "exception.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

%{
#include "server.h"
%}

%include <std_string.i>
%include "server.h"
