#!/usr/bin/python
import socket
import sys
import os

comm_h = file(os.path.join(os.path.dirname(__file__), 'comm.h'))
JOBS_AT_A_TIME = None
PORT = None
for line in comm_h:
	if line.startswith('#define JOBS_AT_A_TIME'):
		JOBS_AT_A_TIME = int(line.rsplit(None, 1)[1])
		sys.stderr.write("Processing %d lines at a time\n" % JOBS_AT_A_TIME)
	if line.startswith('#define PORT'):
		PORT = int(line.rsplit(None, 1)[1])
		sys.stderr.write("Listening on port %d\n" % PORT)
if JOBS_AT_A_TIME is None:
	sys.stderr.write("Cannot find value of JOBS_AT_A_TIME in comm.h\n")
	sys.exit(1)
if PORT is None:
	sys.stderr.write("Cannot find value of PORT in comm.h\n")
	sys.exit(1)

HOST = '0.0.0.0'
if len(sys.argv) > 1:
	HOST = sys.argv[1]
sys.stderr.write("binding to %s\n" % HOST)
allowed = ['127.0.0.1']
PORT = 1477
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((HOST, PORT))
s.listen(1)
while True:
	conn, addr = s.accept()
	print 'Connected by', addr
	if HOST and not addr[0] in allowed and not addr[0].startswith('192.168.'):
		print 'Rejecting %s' % addr[0]
		conn.close()
		continue
	c = 0
	for i in sys.stdin:
		conn.send(i)
		c += 1
		if c == JOBS_AT_A_TIME:
			break
	conn.close()

s.close()
