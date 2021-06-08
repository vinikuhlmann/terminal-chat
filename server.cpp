#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <set>
#include <iterator>
 
using namespace std;

bool done = false;
bool debug = true;
set<int> cl_socket_set; // Stores all the client sockets

int sendmessage()
{
    int  bytes_sent;     // Bytes sent to the socket
    char message[4096];  // Message buffer

    while (true)
    {
        fgets(message, 4096, stdin);                                // Get input
        message[strlen(message)-1] = '\0';                          // Place null terminator at the end of the string
        
        // End the execution if quit() has been typed
        if (strcmp(message, "quit()") == 0)
            exit(0);

        // Send the message to every client
        for (auto cl_socket = cl_socket_set.begin(); cl_socket != cl_socket_set.end(); cl_socket++)
            bytes_sent += send(*cl_socket, message, strlen(message), 0);

        if (debug) cout << "messagge (" << message << ") sent to a client\n";
    }

    done = true;

    if (bytes_sent == -1)
    {
        cerr << "Error in sending message\n";
        return 1;
    }

    return 0;
}

// Listener function, the iterator refers to the position of the socket inside the list of sockets
int listener(set<int>::iterator cl_socket_it)
{
    int bytes_received; // Bytes received at the socket
    char answer[4096];  // Message buffer

    while (!done)
    {
        bytes_received = recv(*cl_socket_it, answer, 4096, 0);  // Receive message
        answer[bytes_received] = '\0';                          // Place null terminator at the end of the string
        cout << answer << endl;                                 // Print message

        // Echo message back to all clients, except the one who sent it
        for (auto cl_socket = cl_socket_set.begin(); cl_socket != cl_socket_set.end(); cl_socket++)
            if (*cl_socket != *cl_socket_it)
                send(*cl_socket, answer, strlen(answer), 0);
        
        // Cliend sent empty message or error, close listener thread
        if (bytes_received <= 0) {
            cl_socket_set.erase(cl_socket_it);
            break;
        }

        else if (debug) cout << "Message received and echoed back to other clients\n";
    }

    if (bytes_received == -1)
    {
        cerr << "Error in receiving message\n";
        return 1;
    }

    done = true;

    return 0;
}

// Connects client to server and starts listening thread
int acceptClient(int sv_socket)
{
    sockaddr_in client;                         // Client's address
    socklen_t client_size = sizeof(client);     // Client's address size

    int cl_socket = accept(sv_socket, (sockaddr*)&client, &client_size);
    if (cl_socket == -1)
    {
        cerr << "Couldn't accept a client's connection\n";
        return 1;
    }

    char host[NI_MAXHOST];                              // Client's remote name
    char service[NI_MAXSERV];                           // Service (i.e. port) the client is connected on
    string hostInfo;                                    // String with client's informations
    memset(host, 0, NI_MAXHOST);                        // Clean the client's remote name memory
    memset(service, 0, NI_MAXSERV);                     // Clean the servece's memory

    // Get the client's remote name, if possible
    stringstream ss;
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
        ss << host << " ";

    // Get the rest of the information
    ss << "(" << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << ")";
    hostInfo = ss.str();

    // Print the host's information
    cout << hostInfo << " connected\n";

    // Insert the socket into the list of client sockets and start server threads
    thread th_listener(listener, cl_socket_set.insert(cl_socket).first);

    if (debug) cout << "listener thread for " << hostInfo << " started\n";

    // Detach threads to allow execution after function ends
    th_listener.detach();

    return 0;
}

/*
*   argv[1] = server IP address
*   argv[2] = server socket port
*/
int main(int argc, char *argv[])
{
    // Get execution commands
    const char *ip = argv[1];
    int port = std::stoi(argv[2]);

    // Make server socket
    int sv_socket;
    sv_socket = socket(AF_INET, SOCK_STREAM, 0);        // SOCK_STREAM for TCP, reliable, connection oriented
    if (sv_socket == -1)
    {
        cerr << "Couldn't create socket\n";
        return 1;
    }

    sockaddr_in hint;                                   // Declare socket address
    hint.sin_family = AF_INET;                          // Store the domain
    hint.sin_port = htons(port);                        // Convert port from host to network short and store
    // hint.sin_addr.s_addr = inet_addr(IP);               Do not do this, inet_addr() is deprecated, do as below
    inet_pton(AF_INET, ip, &hint.sin_addr);             // Store IP address as AF_INET domain
    memset(&hint.sin_zero, 0, sizeof(hint.sin_zero));   // Set sin_zero element to zero

    // Bind the ip address and port to the socket
    if (bind(sv_socket, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        cout << "Couldn't bind socket to an address\n";
        return 2;
    }

    if (debug) printf("Socket created on %s:%d\n", ip, port);

    // Start sendmessage thread
    thread th_sendmessage(sendmessage);
    th_sendmessage.detach();

    if (debug) cout << "sendmessage thread started\n";

    // Listen to clients
    while (!done) {

        if (listen(sv_socket, SOMAXCONN) == -1)
        {
            cerr << "Couldn't prepare to accept connections\n";
            return 3;
        }

        if (debug) cout << "socket set for listening\n";

        // Instantiate function for accepting clients
        thread th_accept(acceptClient, sv_socket);

        if (debug) cout << "acceptClient thread started\n";

        // Wait until a connection has been made to create another thread
        th_accept.join();
    };
 
    return 0;
}