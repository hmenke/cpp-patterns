from __future__ import print_function
import Interface as foo

g = 25

def f(x):
    global g
    tmp = g*x
    g = 13
    return tmp

print("Python:", g)

foo.frontend(f)

print("Python:", g)

foo.frontend(f)
