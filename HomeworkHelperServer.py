from datetime import datetime
from encodings import utf_8
import time
import traceback
import threading
import socket
import os
import sys

# this function is used at the beginning of every print
# returns the current date and time
def getTime():
	return datetime.now().strftime("%m/%d/%Y %H:%M:%S")



def getCurrentFile(Id):
	filename = Id + ".txt"
	# this make its ok to do files stuff once its an exe
	if getattr(sys, 'frozen', False):
		CurrentPath = sys._MEIPASS
	# If it's not use the path we're on now
	else:
		CurrentPath = os.path.dirname(__file__)

	try:
		f = open(filename, "r")

	except FileNotFoundError:
		f = open(filename, "x")
		f.close()
		f = open(filename, "r")
	except:
		print("File error")

	lines = f.read()
		
	return lines


def saveNewFile(Id, newAssignments):
	filename = Id + ".txt"
	try:
		f = open(filename, "w")
		f.write(newAssignments)
		f.close()
	except:
		print("File error")
		f.close()
	

	


# this is the function that handles a new PI connection.
# this function should be run as a thread
# it will continuously wait for data to be sent from 
# the pi and then send that data to the correct connected PC
def handleConnection(connection, piAddr, Id):
	try:
		# first send what we have on file for that Id
		currentFile = getCurrentFile(Id)
		b = currentFile.encode("utf_8")
		l = str(len(b) + 10000000) # add this so that the string is always the same size
		print("{}\tSending message of size {}b to {}".format(getTime(), len(b), connection))
		connection.send(l.encode("utf-8"))
		connection.sendall(b)
		sent = True

		# then continuously update the file as we get new messages
		while True:
			strlen = connection.recv(8).decode("utf-8")
			print(strlen)
			length = int(strlen) - 10000000 # added this so that the bytes size is always the same 
			b = b''
			print("{}\tRecieved message of size {}b from {}".format(getTime(), length, piAddr))
			left = length
			while left != 0:
				batch = min(1024*1024, left)
				newpart = connection.recv(batch)
				left -= len(newpart)
				b += newpart
				print("{}\t{}b out of {}b: {}%".format(getTime(), len(b),length,round((len(b)/length)*10000)/100))
			newAssignments = b.decode("utf-8")
			saveNewFile(Id, newAssignments)


	except:
		print(traceback.format_exc())
		print("{}\tDisconnecting {}".format(getTime(), Id))


# this function will start the server
# waits for a connection from either a PI or a PC
# and handles it accordingly 
def main():
	time.sleep(1)
	print("{}\tStarting server".format(getTime()))
	global connectedPCs
	global connectedPIs
	threads = []
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind(("192.168.1.241", 51153))
	s.listen(5)
	c = []
	while True:
		# Establish connection a client. 
		connection, new_addr = s.accept()
		
		print('{}\tGot connection from {}'.format(getTime(), new_addr))
		Id = connection.recv(1024).decode('utf-8') # this should either be the ID of the connection
		t = threading.Thread(target=handleConnection, args=(connection,new_addr,Id,)) 
		t.start() 
		threads.append(t)


		
if __name__ == '__main__':
	main()