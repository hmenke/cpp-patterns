from PyQt5 import QtCore, QtGui

import time
import zmq
import multiprocessing
import python_zmq_communication as pzc

class Worker(QtCore.QThread):
    def __init__(self, parent=None):
        super().__init__(parent)

    def execute(self, stdout):
        self.stdout = stdout
        self.start()

    def run(self):
        for line in self.stdout:
            print(line)
            if ">>END" in line:
                break

class Communicator:
    def __init__(self, socket):
        self.socket = socket

    def __iter__(self):
        return self

    def __next__(self):
        string = self.socket.recv_string()

        if ">>END" in string:
            raise StopIteration
        else:
            return string

if __name__ == '__main__':
    multiprocessing.freeze_support()
    context = zmq.Context()
    socket = context.socket(zmq.SUB)
    socket.bind("tcp://127.0.0.1:*")
    socket.setsockopt_string(zmq.SUBSCRIBE, u"")
    endpoint = socket.getsockopt_string(zmq.LAST_ENDPOINT)

    print("Binding via {}".format(endpoint))

    thread = Worker()

    proc = multiprocessing.Process(target=pzc.perform, args=(1,endpoint))
    proc.start()
    thread.execute(Communicator(socket))
    thread.wait()
    proc.join()
