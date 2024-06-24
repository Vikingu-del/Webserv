#!/usr/bin/env python3
import cgi
import os

form = cgi.FieldStorage()
file_item = form['file']

if file_item.filename:
    # Define the upload directory
    upload_dir = 'gameHub/database'
    # Create the full path to the uploaded file
    file_path = os.path.join(upload_dir, os.path.basename(file_item.filename))

    # Save the file
    with open(file_path, 'wb') as f:
        f.write(file_item.file.read())

    print("Content-Type: text/html\r\n\r\n")
    print(f"File {file_item.filename} uploaded successfully!")
else:
    print("Content-Type: text/html\r\n\r\n")
    print("No file was uploaded.")
