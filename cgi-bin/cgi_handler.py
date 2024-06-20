#!/usr/bin/env python3

import os
import cgi
import cgitb
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

cgitb.enable()

def send_email(email_address):
    sender_email = "your_email@example.com"
    sender_password = "your_password"
    subject = "GameHub News"
    body = "Check out the latest updates from GameHub!"

    message = MIMEMultipart()
    message["From"] = sender_email
    message["To"] = email_address
    message["Subject"] = subject
    message.attach(MIMEText(body, "plain"))

    try:
        server = smtplib.SMTP("smtp.example.com", 587)  # Replace with your SMTP server
        server.starttls()
        server.login(sender_email, sender_password)
        server.sendmail(sender_email, email_address, message.as_string())
        server.close()
        return "Email sent successfully."
    except Exception as e:
        return f"Error sending email: {str(e)}"

def handle_subscribe():
    form = cgi.FieldStorage()
    if "email" in form:
        email = form.getvalue("email")
        email_status = send_email(email)
        print(email_status)
    else:
        print("Error: No email provided.")

def handle_post():
    form = cgi.FieldStorage()
    if "content" in form:
        content = form.getvalue("content")
        with open("/path/to/database/content.txt", "w") as f:
            f.write(content)
        print("Content posted successfully.")
    else:
        print("Error: No content provided.")

def handle_delete():
    form = cgi.FieldStorage()
    if "id" in form:
        file_id = form.getvalue("id")
        file_path = f"/path/to/database/{file_id}.txt"
        if os.path.exists(file_path):
            os.remove(file_path)
            print("File deleted successfully.")
        else:
            print("Error: File not found.")
    else:
        print("Error: No id provided.")

def main():
    print("Content-Type: text/html")
    print()
    print("<html><body>")

    method = os.environ.get("REQUEST_METHOD", "GET").upper()
    path_info = os.environ.get("PATH_INFO", "")

    if method == "POST":
        if path_info == "/subscribe":
            handle_subscribe()
        elif path_info == "/post":
            handle_post()
        else:
            print("Error: Unknown POST endpoint.")
    elif method == "DELETE":
        handle_delete()
    else:
        print("Unsupported request method.")

    print("</body></html>")

if __name__ == "__main__":
    main()
