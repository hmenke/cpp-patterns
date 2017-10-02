import zmq
import multiprocessing
import python_zmq_communication as pzc

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.bind("tcp://*:*")
socket.setsockopt_string(zmq.SUBSCRIBE, u"")
endpoint = socket.getsockopt_string(zmq.LAST_ENDPOINT)

print("Binding via {}".format(endpoint))

t = multiprocessing.Process(target=pzc.perform, args=(1,endpoint))
t.start()

string = ""
while not ">>END" in string:
    string = socket.recv_string()
    print(string)

t.join()
