%module example

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
    std::string __getstate__() {
      std::stringstream ss;
      boost::archive::binary_oarchive ar(ss);
      ar << *($self);
      return ss.str();
    }

    void __setstate_internal(std::string const &sState) {
      boost::iostreams::array_source asource(sState.c_str(), sState.size());
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
