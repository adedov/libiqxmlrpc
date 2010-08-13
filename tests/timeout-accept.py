import socket, sys, time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('127.0.0.1', 4444))
s.listen(1)
print "Listen port 4444, press enter to accept connection..."
sys.stdin.readline()
s.accept()
