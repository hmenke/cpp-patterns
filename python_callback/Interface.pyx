cdef extern from "CoreFunctions.hpp":
    void cpp_function(object f);

def frontend(f):
    cpp_function(f);
