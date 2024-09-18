HTTP Server in C

- Create a socket
- Bind the socket to the port
- listen to the port for connections
- Establish the connection with the client
- write the code for HTTP methods - GET, POST
- parse the data do something with the data
- Return the response
- free up the resource 
- close the connection

Client
- Create socket
- Connect to server port
- send request and wait for the response
- Accept the response from the server
- Close the connection


To Compile server.c
```
gcc server.c -o server.o
```
To run server
```
./server.o
```