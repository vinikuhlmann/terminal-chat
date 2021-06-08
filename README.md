# Terminal Chat
A c++ terminal program that implements real-time chat between many users through sockets.

### Authors
Vinícius Silva Fernandes Kuhlmann
Pablo Ernani Nogueira de Oliveira - 11215702

### Compilation
The program comes with a makefile, type "make all" in the terminal to compile.

### Execution
Type make newserver to setup a server in 0.0.0.0:54000.
Type make newclient to connect to 0.0.0.0:54000.
Type "./socket IP PORT" to setup a host to specified IP address and port.  
Type "./socket IP PORT" to connect to a server located at the specified IP address and port.  

### Connection and transmission failure verifications
* Create a socket
* Bind the socket to an address
* Listen for clients
* Accept a client's connection request
* Connect to a server
* Send message
* Receive message
