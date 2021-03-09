import socket
import time
import sys


IP_ADDRESS = sys.argv[1]
PORT = int(sys.argv[2])


while True:
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((IP_ADDRESS, PORT))
    message = input("Send to server: ").encode()
    client.send(message)
    print(client.recv(1024))

client.close()



