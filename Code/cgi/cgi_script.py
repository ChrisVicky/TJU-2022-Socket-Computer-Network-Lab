#!/usr/bin/env python3
#
#   This script executes a minimal CGI test function.  Output should be valid
#   HTML and a valid response to the user.  It is an example of a script
#   conforming to RFC 3875 Section: 5.  NPH Scripts.
#
#   Authors: Athula Balachandran <abalacha@cs.cmu.edu>
#            Charles Rang <rang@cs.cmu.edu>
#            Wolfgang Richter <wolf@cs.cmu.edu>

from os import environ
import cgi
import cgitb

cgitb.enable()

print ("HTTP/1.1 200 OK\r\n")
print ("Server: %s\r\n" % (environ['SERVER_SOFTWARE']))

cgi.test()
