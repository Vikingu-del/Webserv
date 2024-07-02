#! /usr/bin/python3

import os
from http import cookies
# import modules for CGI handling
import cgi, cgitb

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
key = form.getvalue('key')
value = form.getvalue('value')

# Check if key or value is None
if key is None or value is None:
    print("HTTP/1.1 400 Bad Request")
    print("Content-Type: text/plain\r\n")
    print("Missing 'key' or 'value' parameter.")
else:
    cookie = cookies.SimpleCookie()
    cookie[key] = value
    print("HTTP/1.1 204 OK")
    print("Content-Type: text/plain")
    print(cookie.output())
    print("\r\n")