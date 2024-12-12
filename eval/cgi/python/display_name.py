#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()  # Enable CGI error reporting

print("Content-Type: text/html")  # HTML is following
print()  # Blank line, end of headers

form = cgi.FieldStorage()

name = form.getvalue("name")

html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Display Name</title>
</head>
<body>
    <h1>Display Name</h1>
    <form method="post" action="/cgi-bin/display_name.py">
        <label for="name">Enter your name:</label>
        <input type="text" id="name" name="name">
        <input type="submit" value="Submit">
    </form>
    <h2>Your name is: {name if name else 'No name provided'}</h2>
</body>
</html>
"""

print(html)