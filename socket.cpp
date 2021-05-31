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
 
using namespace std;

bool done = false;

int sendmessage(int __socket, string type)
{
    int  bytes_sent;     // Bytes sent to the socket
    char message[4096];  // Message buffer

    do
    {
        fgets(message, 4096, stdin);                                // Get input
        message[strlen(message)-1] = '\0';                          // Place null terminator at the end of the string
        bytes_sent = send(__socket, message, strlen(message), 0);   // Send message
    }
    while (bytes_sent > 0);  // Stop if empty message was sent

    done = true;

    if (bytes_sent == -1)
    {
        cout << "Error in sending message\n";
        return 1;
    }

    return 0;
}

int listener(int __socket, string type)
{
    int bytes_received; // Bytes received at the socket
    char answer[4096];   // Message buffer

    do
    {
        bytes_received = recv(__socket, answer, 4096, 0);   // Receive message
        answer[bytes_received] = '\0';                      // Place null terminator at the end of the string
        cout << answer << endl;                             // Print message
    }
    while(bytes_received > 0);    // Stop if empty message was received

    if (bytes_received == -1)
    {
        cout << "Error in receiving message\n";
        return 1;
    }

    done = true;

    return 0;
}

/*
*   argv[1] = "server" for server creation, or "client" for client creation
*   argv[2] = server IP address
*   argv[3] = server socket port
*/
int main(int argc, char *argv[])
{
    // Get execution commands
    const char *type = argv[1];
    const char *ip = argv[2];
    int port = stoi(argv[3]);

    // Make server socket
    int sv_socket;
    sv_socket = socket(AF_INET, SOCK_STREAM, 0);        // SOCK_STREAM for TCP, reliable, connection oriented
    if (sv_socket == -1)
    {
        cout << "Couldn't create socket\n";
        return 1;
    }

    sockaddr_in hint;                                   // Declare socket address
    hint.sin_family = AF_INET;                          // Store the domain
    hint.sin_port = htons(port);                        // Convert port from host to network short and store
    // hint.sin_addr.s_addr = inet_addr(IP);               Do not do this, inet_addr() is deprecated, do as below
    inet_pton(AF_INET, ip, &hint.sin_addr);             // Store IP address as AF_INET domain
    memset(&hint.sin_zero, 0, sizeof(hint.sin_zero));   // Set sin_zero element to zero

    // Initialize server
    if (type == string("server"))
    {
        // Bind the ip address and port to the socket
        if (bind(sv_socket, (sockaddr*)&hint, sizeof(hint)) == -1)
        {
            cout << "Couldn't bind socket to an address\n";
            return 2;
        }

        printf("Socket created on %s:%d\n", ip, port);

        // Listen to a client
        if (listen(sv_socket, SOMAXCONN) == -1)
        {
            cout << "Couldn't prepare to accept connections\n";
            return 3;
        }

        sockaddr_in client;                         // Client's address
        socklen_t client_size = sizeof(client);     // Client's address size

        // Accept client's connection
        int cl_socket = accept(sv_socket, (sockaddr*)&client, &client_size);
        if (cl_socket == -1)
        {
            cout << "Couldn't accept a client's connection\n";
            return 4;
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

        // Start server threads
        thread sv_listener(listener, cl_socket, "server");
        thread sv_sendmessage(sendmessage, cl_socket, "server");

        // Detach so that the threads continue after their handles are destroyed
        sv_listener.detach();
        sv_sendmessage.detach();
    }

    // Connect to existing server
    else if (type == string("client"))
    {
        // Connect to server
        if(connect(sv_socket, (struct sockaddr*)&hint, sizeof(hint)) == -1)
        {
            cout << "Couldn't connect to server\n";
            return 5;
        }

        // Print server information
        printf("Connected to %s:%d\n", ip, port);

        // Start client threads
        thread cl_listener(listener, sv_socket, "client");
        thread cl_sendmessage(sendmessage, sv_socket, "client");

        // Detach so that the threads continue after their handles are destroyed
        cl_listener.detach();
        cl_sendmessage.detach();
    }

    else
    {
        cout << type << " is not a valid command\n";
        return 0;
    }

    while (!done) {};
 
    return 0;
}