
from gevent import socket
import multiprocessing


def process_main():

	for i in range(10000):
		conn = socket.create_connection(('localhost', 9977))
		conn.send("ABCDE")
		conn.recv(5);
		conn.close()



num_processes = 1
processes = []

for i in range(num_processes):
		p = multiprocessing.Process(target=process_main)
		p.start()
		processes.append(p);

for i in range(num_processes):
		processes[i].join()
		
