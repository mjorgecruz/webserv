#!/usr/bin/env python3

import cgi
import cgitb
import os
import time

# Enable debugging
cgitb.enable()

# Get the form data
form = cgi.FieldStorage()

while True:
    time.sleep(1) 