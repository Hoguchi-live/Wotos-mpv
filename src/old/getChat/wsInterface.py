import json
import queue
import requests
import websocket

from helper import *

import sys
import socket
import fcntl, os

SEND_BUFFER_SIZE = 2048
connected = 0

try: import thread
except ImportError: import _thread as thread

class Socket_:
    def __init__(self, queue = None):
        self.URL = None
        self.sid = None
        self.sidWSS = None
        self.parser = Parser().parser
        self.queue = queue
        self.ws = None

        self.sock = None

        # Start the connection
        self.handshake()

    def handshake(self):
        # First, establish a polling-transport HTTP connection
        resp_1 = requests.get(POLLING_URL_1, headers = HEADER)
        print('Resp 1: ', resp_1.text)
        # Ignore the bin-packed preliminaries and extract the session ID
        # WOTOS: self.sid = json.loads(str(resp_1.content)[3:-1])['sid']
        self.sid = json.loads(str(resp_1.content)[3:-1])['sid']
        # Second polling with POST, response should be 'ok'
        resp_2 = requests.post(POLLING_URL_2.format(self.sid), headers = HEADER, data = b'40')
        print('Resp 2: ', resp_2.text)
        # Third polling
        resp_3 = requests.get(POLLING_URL_3.format(self.sid), headers = HEADER)
        print('Resp 3: ', resp_3.text)
        # WOTOS: self.sid = json.loads(str(resp_1.content)[3:-1])['sid']
        index = str(resp_3.content).find('\\x1e42[')
        substr = str(resp_3.content)[4:index]
        self.sidWSS = json.loads(substr)['sid']
        # Fourth polling with POST
        resp_4 = requests.post(POLLING_URL_4.format(self.sid), headers = HEADER, data = b'42["joinChannel",{"name":"testam"}]')
        print('Resp 4: ', resp_4.text)
        # Fourth polling with GET
        resp_5 = requests.get(POLLING_URL_5.format(self.sid), headers = HEADER)
        print('Resp 5: ', resp_5.text)


        # WSS url
        self.URL = WSS_URL.format(self.sid)
        print(self.URL)

    def on_open(self, ws):
        print('### Socket open ###')
        ws.send("2probe")
    def on_close(self, ws):
        print("### Socket closed ###")
        self.sock.close()
    def on_error(self, ws, error):
        print('\nSocket error :\n{}\n'.format(error))

    def on_message(self, ws, message):
        if message == '2': ws.send('3')
        elif message == '3probe': ws.send('5')
        elif message[:2] == '42':
            j = message[2:].replace("'", '"')
            parsed = json.loads(j)
            print("received: ", message)
            reason = parsed[0]
            if reason == 'chatMsg':
                data = bytes(parsed[1]['msg'], "utf-8")
                data += bytes(SEND_BUFFER_SIZE-len(data))
                self.sock.send(data)
                print(data)
            else: pass

    def listenForever(self):
        # SOCKET
        server_ip = "localhost"
        server_port = 10000
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((server_ip, server_port))
        fcntl.fcntl(self.sock, fcntl.F_SETFL, os.O_NONBLOCK)

        try:
            #websocket.enableTrace(True)
            self.ws = websocket.WebSocketApp(self.URL,
                            on_open = self.on_open,
                            on_message = self.on_message,
                            on_error = self.on_error,
                            on_close = self.on_close,
                            header = HEADER_WSS
                            )
            self.ws.run_forever(ping_interval = 25, ping_timeout = 5 )
        except Exception as e:
            print("Socket::ListenForever: Exception {}", format(e))