#!/usr/bin/python3

import cgi
import os

# Set the directory where the subscription emails will be saved
SUBSCRIPTION_DIR = os.path.join(os.getcwd(), 'subscriptions')
os.makedirs(SUBSCRIPTION_DIR, exist_ok=True)

def save_subscription(email):
    try:
        # Save the email to a file
        filepath = os.path.join(SUBSCRIPTION_DIR, 'subscribers.txt')
        with open(filepath, 'a') as file:
            file.write(f"{email}\n")
        return "Subscription successful!"
    except Exception as e:
        return f"An error occurred: {str(e)}"

def main():
    # Get the form data
    form = cgi.FieldStorage()
    email = form.getvalue('email')
    message = "Subscription failed."

    if email:
        message = save_subscription(email)
    else:
        message = "No email provided."

    # Return the response
    print("Content-Type: text/html;charset=utf-8")
    print()
    print(f"<html><body><h1>{message}</h1></body></html>")

if __name__ == "__main__":
    main()