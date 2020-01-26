#include "fileptrbuf.hpp"

#include <iostream>
#include <sstream>

int main() {
    {
        FILE *f = fopen("foo.txt", "w");
        fileptrbuf fb(f, std::ios::out);
        std::ostream os(&fb);
        os << "foo" << std::endl;
        fclose(f);
    }
    {
        FILE *f = fopen("foo.txt", "rw");
        fileptrbuf fb(f, std::ios::in);
        std::istream is(&fb);
        std::stringstream buffer;
        buffer << is.rdbuf();
        std::cout << buffer.str() << std::endl;
        fclose(f);
    }
}

