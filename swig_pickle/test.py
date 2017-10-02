import pickle
import swig_pickle as sp

t = sp.Test()
t.construct()

print(t.get())

with open("test.dat", "wb") as f:
    pickle.dump(t,f)

del(t)

with open("test.dat", "rb") as f:
    t = pickle.load(f)

print(t.get())
