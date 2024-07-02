#! /usr/bin/python3

from http import cookies
import os  #["HTTP_COOKIE"] is used to retrieve cookie data sent by the client via the HTTP Cookie header.

def show_cookie(dictionary): # designed to output the name and value of each cookie passed in the dictionary parameter, formatted for the web page.
    for key, morsel in dictionary.items():
        print(key, ':', morsel.value, '<br>')
#basicly this function is used to print the cookies in the web page

cookie = cookies.BaseCookie() #create a cookie object
print("Content-type: text/html\r\n") #print the content(cookie) type
if 'HTTP_COOKIE' in os.environ: #check if the cookie is in the environment
    cookie.load(os.environ['HTTP_COOKIE']) #load the cookie
    show_cookie(cookie) #print the cookie
else:
    print("No cookies")
