import zmq
import multiprocessing
import example as ex

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.bind("tcp://*:*")
socket.setsockopt_string(zmq.SUBSCRIBE, u"")
endpoint = socket.getsockopt_string(zmq.LAST_ENDPOINT)

print("Binding via {}".format(endpoint))

t = multiprocessing.Process(target=ex.perform, args=(1,endpoint))
t.start()

string = ""
while not ">>END" in string:
    string = socket.recv_string()
    print(string)

t.join()
