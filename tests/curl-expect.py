import pycurl
import sys
import time

s = pycurl.Curl()
s.setopt(s.URL, sys.argv[1])
body = 'a'*65535
s.setopt(s.POSTFIELDS, """<?xml version="1.0"?>
<methodCall>
  <methodName>echo</methodName>
  <params>
    <param>
      <value>
        <string>%s</string>
      </value>
    </param>
  </params>
</methodCall>""" % (body))
s.setopt(s.VERBOSE, True)
s.setopt(s.HTTPHEADER, ["Content-Type: text/xml"])
s.setopt(pycurl.SSL_VERIFYPEER, 0)
s.setopt(pycurl.SSL_VERIFYHOST, 0)

t1 = time.time()
s.perform()
t2 = time.time()

timeout = t2-t1
if timeout > 1.0:
	print "Expect timeout: %fs" % timeout
else:
	print "OK"
