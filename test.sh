#!/bin/bash

# Define the URL and port
URL="localhost"
PORT="8080"
PATH="/directory/youpi.bla"

# Define the size of the payload
PAYLOAD_SIZE=10000

# Open a raw socket connection to the server
exec 3<>/dev/tcp/$URL/$PORT

# Send the HTTP headers
echo -ne "POST $PATH HTTP/1.1\r\n" >&3
echo -ne "Host: $URL:$PORT\r\n" >&3
echo -ne "Transfer-Encoding: chunked\r\n" >&3
echo -ne "Content-Type: application/octet-stream\r\n" >&3
echo -ne "\r\n" >&3

# Generate the payload in chunks
CHUNK_SIZE=4096
for ((i=0; i<$PAYLOAD_SIZE; i+=$CHUNK_SIZE)); do
  if (( $PAYLOAD_SIZE - $i < $CHUNK_SIZE )); then
    CHUNK_SIZE=$(( $PAYLOAD_SIZE - $i ))
  fi
  printf '%x\r\n' $CHUNK_SIZE >&3
  dd if=/dev/zero bs=$CHUNK_SIZE count=1 2>/dev/null >&3
  echo -ne "\r\n" >&3
done

# End of chunks
echo -ne "0\r\n\r\n" >&3

# Close the socket
exec 3>&-