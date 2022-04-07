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



qstring = environ["QUERY_STRING"] 
uName = qstring[qstring.find('=')+1:qstring.find('&')]
qstring = qstring[qstring.find('&'):]
uPass = qstring[qstring.find('=')+1:]
uIp = environ["REMOTE_ADDR"]
method = environ["REQUEST_METHOD"]
Response = {}
dic = {}

dic['Ip'] = uIp
dic['method'] = method

if len(uName)==0 or len(uPass)==0:
    Response['Code'] = "400"
    Response['Msg'] = "Invalid User Name or User Password"
    dic['Ip'] = uIp
    dic['method'] = method
    Response['Results'] = dic
    Response = json.dumps(Response)
    print(Response)
    exit(0)
 
dic['name'] = uName
dic['password'] = uPass

try:
    db = pymysql.connect(host="8.141.166.181", user="twt_vote", password="TWT_vote1895", database="socket_db")
except:
    Response['Code'] = "400"
    Response['Msg'] = "SQL Connection Error"
    Response['Results'] = dic
    Response = json.dumps(Response)
    print(Response)
    exit(1)

cursor = db.cursor()
SLC_NAME = f"SELECT * FROM users WHERE u_name='{uName}'"
try:
    cursor.execute(SLC_NAME)
    results = cursor.fetchall()
except:
    Response['Code'] = "400"
    Response['Msg'] = "SQL Connection Error"
    Response['Results'] = dic
    Response = json.dumps(Response)
    print(Response)
    exit(1)

if len(results)!=0:
    # User Name has been Registered before.

    PASS = results[0][2]
    nT = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    Response['Code'] = "400"
    dic['Ip'] = uIp
    dic['method'] = method
    dic['name'] = uName

    try:
        cursor.execute(f"SELECT created_at FROM users WHERE u_name='{uName}'")
        result = cursor.fetchall()[0][0].strftime("%Y-%m-%d %H:%M:%S")
        Response['Msg'] = f"User Name Registered at {result}."
        dic['created_at'] = result
    except:
        Response['Code'] = "400"
        Response['Msg'] = "SQL Connection Error"
        Response['Results'] = dic
        Response = json.dumps(Response)
        print(Response)
        exit(1)


else:
    # Insert into Database
    nT = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())    
    IST_New = f"INSERT INTO users (u_name, u_pass, ip, created_at, updated_at) VALUES ('{uName}','{uPass}','{uIp}','{nT}','{nT}')"
    try:
        cursor.execute(IST_New)
        db.commit()
    except:
        db.rollback()
        exit(1)
    
    Response['Code'] = "200"
    Response['Msg'] = "You are now registered"

    dic['Ip'] = uIp
    dic['method'] = method
    dic['name'] = uName
    dic['pass'] = uPass
    dic['created_at'] = nT
    dic['updated_at'] = nT

    try:
        cursor.execute(f"SELECT u_id FROM users WHERE u_name='{uName}'")
        result = cursor.fetchall()[0][0]
        dic['number'] = result
    except:
        exit(1)

db.close()
Response['Results'] = dic
Response = json.dumps(Response)
print(Response)


