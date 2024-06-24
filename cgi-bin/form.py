#!/usr/bin/env python

import cgi
import cgitb
import os
import json

# Enable debugging
cgitb.enable()

# Path to the database file
DB_PATH = "./gameHub/database/data.json"
UPLOAD_DIR = "./gameHub/database/uploads"

def load_database():
    if not os.path.exists(DB_PATH):
        return {}
    with open(DB_PATH, 'r') as db_file:
        return json.load(db_file)

def save_database(data):
    with open(DB_PATH, 'w') as db_file:
        json.dump(data, db_file, indent=4)

def handle_upload(form):
    if "image" not in form:
        return "Missing image", 400
    
    fileitem = form["image"]
    
    if not fileitem.file:
        return "No file uploaded", 400
    
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)
    
    filepath = os.path.join(UPLOAD_DIR, fileitem.filename)
    
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())
    
    database = load_database()
    if "uploads" not in database:
        database["uploads"] = []
    
    database["uploads"].append({"filename": fileitem.filename, "path": filepath})
    save_database(database)
    
    return "Upload successful!", 200

def main():
    print("Content-Type: text/html\n")

    # Debugging: print all environment variables
    print("<html><body><pre>")
    for key, value in os.environ.items():
        print("{}: {}".format(key, value))
    print("</pre>")
    
    # Check for REQUEST_METHOD
    if 'REQUEST_METHOD' not in os.environ:
        print("<h1>Error: REQUEST_METHOD not set</h1>")
        print("</body></html>")
        return

    form = cgi.FieldStorage()
    method = os.environ['REQUEST_METHOD']

    if method == "POST":
        path = os.environ.get('PATH_INFO', '')
        if path == '/subscribe':
            message, status = handle_subscribe(form)
        elif path == '/post':
            message, status = handle_post(form)
        elif path == '/delete':
            message, status = handle_delete(form)
        elif path == '/upload':
            message, status = handle_upload(form)
        else:
            message, status = "Not Found", 404
    else:
        message, status = "Method Not Allowed", 405

    print("Status: {}".format(status))
    print()
    print("<h1>{}</h1>".format(message))
    print("</body></html>")

if __name__ == "__main__":
    main()
