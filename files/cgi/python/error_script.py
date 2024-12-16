#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()  # Enable CGI error reporting

print("Content-Type: text/html")  # HTML is following
print()  # Blank line, end of headers

form = cgi.FieldStorage()

# Deliberate mistake: using an undefined variable 'message'
html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Error Script</title>
</head>
<body>
    <h1>Error Script</h1>
    <p>Your message is: {message}</p>  <!-- 'message' is not defined -->
</body>
</html>
"""

print(html)