import socket
import ssl

r = """POST / HTTP/1.1
User-Agent: curl/7.30.0
Host: localhost:3344
Accept: */*
Content-type:text/xml
Content-Length: 268

<?xml version="1.0"?><methodCall>  <methodName>get_file</methodName>  <params>    <param>      <value>	<struct>	<member>	  <name>requested-size</name>	  <value><int>55000</int></value>        </member>        </struct>      </value>    </param>  </params></methodCall>"""

s = ssl.wrap_socket( socket.socket(socket.AF_INET, socket.SOCK_STREAM) )
s.connect(('127.0.0.1', 3344))
s.send(r)

print "Close prematurely"
s.close()
#rv = s.recv()
