#!/usr/bin/env python3

""" client.py - Echo client for sending/receiving C-like structs via socket
References:
- Ctypes: https://docs.python.org/3/library/ctypes.html
- Sockets: https://docs.python.org/3/library/socket.html
"""


import threading
import socket
import queue
import sys
import time
import random
from ctypes import *

DATE_FORMAT = "%d/%m/%Y %H:%M:%S"
SERVER_REFRESH_RATE = 100

""" This class defines a C-like struct """
class Payload(Structure):
    _fields_ = [("id", c_uint32),
                ("counter", c_uint32),
                ("data", c_wchar_p)]

class CSV_Socket(threading.Thread):
    def __init__(self,  queue, *args, **kwargs):
        super(CSV_Socket, self).__init__(*args, **kwargs)
        self._stop_event = threading.Event()
        self.queue = queue
        self.is_on = False

        self.server_addr = ('localhost', 2300)
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def run(self):
        print("::Running CSV_Server")
        self.is_on = True

        try:
            self.socket.connect(self.server_addr)
            print("Connected to {:s}".format(repr(self.server_addr)))

            c = 0
            while self.is_on:
                if not self.queue.empty():
                    raw_data = self.queue.get() + "\0"
                    payload_out = Payload(c, len(raw_data), raw_data)
                    print("Sending id={:d}, counter={:d}, data={}".format(payload_out.id,
                                                                      payload_out.counter,
                                                                      payload_out.data))
                    nsent = self.socket.send(payload_out)
                    # Alternative: s.sendall(...): coontinues to send data until either
                    # all data has been sent or an error occurs. No return value.
                    print("Sent {:d} bytes".format(nsent))

                    #buff = s.recv(sizeof(Payload))
                    #payload_in = Payload.from_buffer_copy(buff)
                    #print("Received id={:d}, counter={:d}, temp={}".format(payload_in.id,
                    #                                                payload_in.counter,
                    #                                                payload_in.data))

                    c += 1
                time.sleep(SERVER_REFRESH_RATE*0.001)
        except AttributeError as ae:
            print("Error creating the socket: {}".format(ae))
        except socket.error as se:
            print("Exception on socket: {}".format(se))
        finally:
            print("Closing socket")
            self.socket.close()

    def stop(self):
        print("::Stopping Server")
        self.is_on = False
        self._stop_event.set()

    def toggle(self):
        if self.is_on:
            print("::Pausing Server")
            self.is_on = False
        elif not self.is_on:
            self.run()

    def stopped(self):
        return self._stop_event.is_set()
