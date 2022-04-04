#!/usr/bin/env python3
#
#   This script does not provide a valid CGI response or a valid HTTP/1.1
#   response.  In fact, it does not even produce valid HTML.  It is meant to
#   help you investigate the variables and other parameters your server is
#   passing to the script.  You may want to turn the prints into writes to a
#   a log file especially when you start redirecting stdout of the script.
#
#   Authors: Athula Balachandran <abalacha@cs.cmu.edu>
#            Charles Rang <rang@cs.cmu.edu>
#            Wolfgang Richter <wolf@cs.cmu.edu>

from os import environ

qstring = environ["QUERY_STRING"] 
uName = qstring[qstring.find('=')+1:qstring.find('&')]
qstring = qstring[qstring.find('&'):]
uPass = qstring[qstring.find('=')+1:]
uIp = environ["REMOTE_ADDR"]

print (f"Hello Mr/Ms {uName} from {uIp}, your Password is {uPass}.")



