#!/usr/bin/env python3

import cgi
import cgitb

# Enable CGI error reporting
cgitb.enable()

print("Content-Type: text/html")    # HTML is following
print()                             # blank line, end of headers
print()                             # blank line, end of headers

print("<html>")
print("<head>")
print("<title>Python CGI Script</title>")
prt("</head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")

# Get form data
form = cgi.FieldStorage()

# Check if the form has a 'name' field
if "name" in form:
    name = form.getvalue("name")
    print(f"<p>Hello, {name}!</p>")
else:
    print("<p>Hello, World!</p>")

print("</body>")
print("</html>")