#!/usr/bin/python3

import os, cgi

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
    # strip leading path from file name to avoid directory traversal attacks
    open(os.getcwd() + '/cgi-bin/tmp/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
    message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded successfully'
else:
    message = 'Uploading Failed'

print("content-type: text/html; charset=utf-8")
print("content-type: text/html\r\n")
print("<H1> " + message + " </H1>")