import socket
import time
import sys

server = "irc.rizon.net"       #settings
channel = "#soranowoto"
botnick = "assam__"

irc = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #defines the socket
print("connecting to:"+server)

irc.connect((server, 6667)) 
irc.send(bytes("NICK "+ botnick +"\n", encoding='utf8'))
irc.send(bytes("PASS PekoRrat\n", encoding='utf8'))
irc.send(bytes("USER "+ botnick +" " + "assam assam " + "assam\n", encoding='utf8'))
time.sleep(5)
irc.send(bytes("JOIN "+ channel +"\n", encoding='utf8'))

while 1:
   text=irc.recv(2040)
   print(text)

   if text.find(bytes('PING', encoding='utf8')) != -1:                          #check if 'PING' is found
       irc.send(bytes('PONG ', encoding='utf8') + text.split()[1] + bytes('\r\n', encoding='utf8') )

