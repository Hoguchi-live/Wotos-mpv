import socket, string, time, thread
SERVER = 'irc.rizon.net'
PORT = 6667
NICKNAME = 'GataGoto'
CHANNEL = '#soranowoto'

def main():
	global IRC
	IRC = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	IRC.connect((SERVER, PORT))
	thread.start_new_thread(Listener(),("Thread No:1",2))

def send_data(command):
    IRC.send(command + '\n')

def Listener():
	send_data('USER Blah')
	send_data('NICK Blah')
	while (1):
		buffer = IRC.recv(1024)
		msg = string.split(buffer)
		if msg[0] == "PING":
			print('Pinged!')
			IRC.send("PONG %s" % msg[1] + '\n')

main()
