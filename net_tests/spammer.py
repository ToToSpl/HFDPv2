import socket
import time


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.bind(("127.0.0.1", 10001))

try:
    while True:
        sock.sendto("hello from hfdp!!!".encode("utf-8"), ("127.0.0.1", 10000))
        time.sleep(0.05)
except KeyboardInterrupt:
    print("\nclosing...")


sock.close()