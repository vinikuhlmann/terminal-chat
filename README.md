# Terminal Chat
A c++ terminal program that implements real-time chat between many users through sockets.

### Authors
Vinícius Silva Fernandes Kuhlmann - 11215751  
Pablo Ernani Nogueira de Oliveira - 11215702  

###
Compiled on g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0

### Compilation
The program comes with a makefile, type "make all" in the terminal to compile.

### Execution
Type make newserver to setup a server in 0.0.0.0:54000.
Type make newclient to connect to 0.0.0.0:54000.
Type "./server IP PORT" to setup a host to specified IP address and port.  
Type "./client IP PORT" to connect to a server located at the specified IP address and port.  

### Connection and transmission failure verifications
* Create a socket
* Bind the socket to an address
* Listen for clients
* Accept a client's connection request
* Connect to a server
* Send message
* Receive message
