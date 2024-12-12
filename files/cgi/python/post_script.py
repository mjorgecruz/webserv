#!/usr/bin/env python3

import cgi
import cgitb
import os

# Enable debugging
cgitb.enable()

# Get the form data
form = cgi.FieldStorage()

# Define the path to save the messages
MESSAGE_FILE = "/tmp/cgi_messages.txt"

def load_messages():
    """Load existing messages from the file."""
    if not os.path.exists(MESSAGE_FILE):
        return []
    with open(MESSAGE_FILE, "r") as f:
        return f.readlines()

def save_message(message):
    """Save a new message to the file."""
    with open(MESSAGE_FILE, "a") as f:
        f.write(message + "\n")

# Read the posted message
message = form.getvalue("message")
if message:
    save_message(message)

# Load all messages to display
messages = load_messages()

# Generate the HTML output
print("Content-type: text/html\n")
print("<html>")
print("<head>")
print("<title>CGI Message Board</title>")
print("</head>")
print("<body>")
print("<h1>Post a Message</h1>")
print("<form method='post' action='/files/cgi/python/post_script.py'>")
print("    <textarea name='message' rows='4' cols='50'></textarea><br>")
print("    <input type='submit' value='Post Message'>")
print("</form>")

print("<h2>Messages:</h2>")
if messages:
    print("<ul>")
    for msg in messages:
        print(f"<li>{msg.strip()}</li>")
    print("</ul>")
else:
    print("<p>No messages yet.</p>")

print("</body>")
print("</html>")
