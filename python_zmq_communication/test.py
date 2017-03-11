import zmq
import multiprocessing
import example as ex

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.bind("tcp://*:5556")
socket.setsockopt_string(zmq.SUBSCRIBE, u"")

t = multiprocessing.Process(target=ex.perform, args=[1])
t.start()

string = ""
while not ">>END" in string:
    string = socket.recv_string()
    print(string)

t.join()
