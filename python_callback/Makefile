CXX = c++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fPIC

all: Interface.so

Interface.cpp: Interface.pyx
	cython --cplus Interface.pyx

Interface.o: Interface.cpp
	$(CXX) -std=c++11 -fPIC -I/usr/include/python2.7 -c Interface.cpp

CoreFunctions.o: CoreFunctions.cpp CoreFunctions.hpp PythonCallback.hpp
	$(CXX) $(CXXFLAGS) -I/usr/include/python2.7 -c CoreFunctions.cpp

PythonCallback.o: PythonCallback.cpp PythonCallback.hpp
	$(CXX) $(CXXFLAGS) -I/usr/include/python2.7 -c PythonCallback.cpp

Interface.so: Interface.o CoreFunctions.o PythonCallback.o
	$(CXX) $(CXXFLAGS) -shared -o Interface.so CoreFunctions.o Interface.o PythonCallback.o

clean:
	rm Interface.cpp Interface.o CoreFunctions.o Interface.so PythonCallback.o
