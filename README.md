# Terminal Chat
A c++ terminal program that implements real-time chat between many users through sockets.

### Authors
Vinícius Silva Fernandes Kuhlmann

### Compilation
The program comes with a makefile, type "make all" in the terminal to compile

### Execution
Type make server to setup a server in 0.0.0.0:54000.  
Type make client to connect to 0.0.0.0:54000.  
Type "./socket server IP PORT" to setup a host to specified IP address and port.  
Type "./socket client IP PORT" to connect to a server located at the specified IP address and port.  

### Features
- [x] TCP connection between terminals
- [ ] Multiple connections in one server
- [ ] Author and datetime information on messages
- [ ] Store messages in log.txt

### Connection and transmission failure verifications
* Create a socket
* Bind the socket to an address
* Listen for client's
* Accept a client's connection request
* Connect to a server
* Send messages
* Receive message