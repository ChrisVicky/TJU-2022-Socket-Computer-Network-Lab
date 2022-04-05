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
import pymysql
import time
import json
import datetime



qstring = environ["QUERY_STRING"] 
uName = qstring[qstring.find('=')+1:qstring.find('&')]
qstring = qstring[qstring.find('&'):]
uPass = qstring[qstring.find('=')+1:]
uIp = environ["REMOTE_ADDR"]
method = environ["REQUEST_METHOD"]
Response = {}
dic = {}


if len(uName)==0 or len(uPass)==0:
    dic['Ip'] = uIp
    dic['method'] = method
    Response['Code'] = "400"
    Response['Msg'] = "Invalid User name or Password"
    Response['Results'] = dic
    Response = json.dumps(Response)
    print(Response)
    exit(0)

db = pymysql.connect(host="8.141.166.181", user="twt_vote", password="TWT_vote1895", database="socket_db")
cursor = db.cursor()
SLC_NAME = f"SELECT * FROM users WHERE u_name='{uName}'"
cursor.execute(SLC_NAME)
results = cursor.fetchall()
if len(results)!=0:
    # User Name has been Registered before.

    PASS = results[0][2]
    nT = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    if(PASS != uPass):
        Response['Code'] = "400"
        Response['Msg'] = "Error Password"
    else:
        Response['Code'] = "200"
        Response['Msg'] = "User Name Registered before."
        try:    
            cursor.execute(f"SELECT ip FROM users WHERE u_name='{uName}'")
            result = cursor.fetchall()[0]
            dic['lastIp'] = result
            cursor.execute(f"UPDATE users SET updated_at = '{nT}', ip='{uIp}' WHERE u_name='{uName}'")
            db.commit()
            dic['updated_at'] = nT
        except:
            Response['Msg'] = "Error Updating DB"
            Response['Code'] = "400"
            db.rollback()
        
        cursor.execute(f"SELECT created_at FROM users WHERE u_name='{uName}'")
        result = cursor.fetchall()[0]
        dic['created_at'] = result[0].strftime("%Y-%m-%d %H:%M:%S")
else:
    Response['Code'] = "400"
    Response['Msg'] = "You have to register before login"

dic['Ip'] = uIp
dic['method'] = method
dic['name'] = uName

db.close()
Response['Results'] = dic
Response = json.dumps(Response)
print(Response)


