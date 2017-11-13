%module swig_pickle

%{
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <sstream>

#include "test.h"
%}

%include "std_string.i"

// https://stackoverflow.com/a/19798903
%define %boost_picklable(cls...)
  %extend cls {
    PyObject * __getstate__() {
      std::stringstream ss;
      boost::archive::binary_oarchive ar(ss);
      ar << *($self);
      return PyBytes_FromStringAndSize(ss.str().data(), ss.str().length());
    }

    void __setstate_internal(PyObject * const sState) {
      char *buffer;
      Py_ssize_t len;
      PyBytes_AsStringAndSize(sState, &buffer, &len);
      boost::iostreams::array_source asource(buffer, static_cast<size_t>(len));
      boost::iostreams::stream<boost::iostreams::array_source> ss(asource);
      boost::archive::binary_iarchive ar(ss);
      ar >> *($self);
    }

    %pythoncode %{
      def __setstate__(self, sState):
          self.__init__()
          self.__setstate_internal(sState)
    %}
  }
%enddef

%include "test.h"
%boost_picklable(Test)
