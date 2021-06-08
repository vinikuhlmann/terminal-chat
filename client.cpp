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
*   argv[1] = server IP address
*   argv[2] = server socket port
*/
int main(int argc, char *argv[])
{
    // Get execution commands
    const char *ip = argv[1];
    int port = stoi(argv[2]);

    // Make socket
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

    // Connect to server
    if(connect(sv_socket, (struct sockaddr*)&hint, sizeof(hint)) == -1)
    {
        cout << "Couldn't connect to server\n";
        return 5;
    }

    // Print server information
    printf("Connected to %s:%d\n", ip, port);

    // Start client threads
    thread th_listener(listener, sv_socket, "client");
    thread th_sendmessage(sendmessage, sv_socket, "client");
    th_listener.detach();
    th_sendmessage.detach();

    while (!done) {};
 
    return 0;
}