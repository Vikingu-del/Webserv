#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Initialize message
message = 'Uploading Failed'

# Ensure the directory exists
upload_dir = os.path.join(os.getcwd(), 'cgi-bin', 'tmp')
os.makedirs(upload_dir, exist_ok=True)

# Test if the file was uploaded
if fileitem.filename:
    # Sanitize the file name
    filename = os.path.basename(fileitem.filename)
    filepath = os.path.join(upload_dir, filename)
    
    try:
        # Write the file content in binary mode
        with open(filepath, 'wb') as file:
            file.write(fileitem.file.read())
        message = f'The file "{filename}" was uploaded to {upload_dir}'
    except Exception as e:
        message = f'An error occurred: {str(e)}'

print("Content-Type: text/html;charset=utf-8\r\n")
print(f"<H1>{message}</H1>")