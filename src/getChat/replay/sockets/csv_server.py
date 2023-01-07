from csv_reader import *
from csv_socket import *

def main():
    Q = queue.Queue()

    S = CSV_Socket(Q)
    R = CSV_Reader(Q)

    S.start()
    R.start()

    try:
        while 1:
            time.sleep(0.1)
    except KeyboardInterrupt:
        S.stop()
        R.stop()

if __name__ == "__main__":
    main()
