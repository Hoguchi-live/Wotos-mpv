# -*- coding: UTF-8 -*-
# Reader
from datetime import datetime
import threading
import queue
import time
import sys

import csv

import sys
import socket
import fcntl, os


FILENAME = "data/danmaku.csv"
DATE_FORMAT = "%d/%m/%Y %H:%M:%S"
READER_REFRESH_RATE = 100
SEND_BUFFER_SIZE = 2048

class CSV_Reader(threading.Thread):
        def __init__(self,  queue, videoStartTime, *args, **kwargs):
            super(CSV_Reader, self).__init__(*args, **kwargs)

            self._stop_event = threading.Event()
            self.queue = queue
            self.is_on = False

            self.video_zero_time = videoStartTime
            self.video_zero_time_relative = None
            self.danmaku_zero_time = None
            self.danmaku_time_delta = 0

            # SOCKET
            self.server_ip = "localhost"
            self.server_port = 10000
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.server_ip, self.server_port))
            fcntl.fcntl(self.sock, fcntl.F_SETFL, os.O_NONBLOCK)

        def run(self):
            print("::Running CSV_Reader")
            self.is_on = True

            with open(FILENAME, newline='') as csvfile:
                spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')

                while self.is_on:
                    raw_time, raw_data = next(spamreader)
                    d_time = datetime.strptime(raw_time, DATE_FORMAT)
                    ts_time = int(datetime.timestamp(d_time))

                    # Set starting point
                    if self.danmaku_zero_time == None:
                        self.video_zero_time_relative = time.time()
                        self.video_zero_time_zero = ts_time
                        self.danmaku_zero_time = ts_time

                    video_elapsed = 0
                    delta_time = 1
                    while delta_time > video_elapsed:
                        # Time elapsed relative to the video
                        video_elapsed = time.time() - self.video_zero_time_relative
                        delta_time = ts_time - self.danmaku_zero_time

                        # Sleep
                        time.sleep(READER_REFRESH_RATE*0.001)

                    print("time: {} ## data: {} ## delta: {}".format(raw_time, raw_data, delta_time))
                    data = bytes(raw_data, "utf-8")
                    data += bytes(SEND_BUFFER_SIZE-len(data))
                    self.sock.send(data)

        def stop(self):
            print("::Stopping Watcher")
            self.is_on = False
            self._stop_event.set()

        def toggle(self):
            if self.is_on:
                print("::Pausing Watcher")
                self.is_on = False
            elif not self.is_on:
                self.run()

        def stopped(self):
            return self._stop_event.is_set()

def main():
    videoStartTime = 0
    if len(sys.argv) == 2:
        VideoStartTime = sys.argv[1]

    Q = queue.Queue()
    R = CSV_Reader(Q, videoStartTime)
    R.start()

if __name__ == "__main__":
    main()
