from socket import *
import ssl

fd = ssl.wrap_socket(socket(AF_INET, SOCK_STREAM))
ip = gethostbyname('www.runoob.com')
port = 443

fd.connect(('www.runoob.com', port))

msg = '''GET /try/ajax/json_demo.json HTTP/1.1\r
Host: www.runoob.com\r
Connection: close\r
\r\n'''

#print msg
fd.send(msg.encode())
res = fd.recv(1024)

print res
fd.close()

