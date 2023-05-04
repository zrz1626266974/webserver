from socket import *
#import ssl
fd = socket(AF_INET, SOCK_STREAM)
addr = ('localhost', 8888)
fd.connect(addr)



