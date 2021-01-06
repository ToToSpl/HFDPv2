import socket
import time


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("127.0.0.1", 10000))

try:
    while True:
        data = sock.recv(1024)
        print(data)
except KeyboardInterrupt:
    print("\nclosing...")


sock.close()