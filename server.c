#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // contains constants and structures needed for domain addrs and other such stuff
#include <unistd.h>

void error(const char *msg)
{
    perror(msg); // uses stderr
    exit(1);
}

int main(int argc, char *argv[])
{
    // if command line arguments are missing terminate the program
    if (argc < 2)
    {
        fprintf(stderr, "Port number missing. Program terminated \n");
        exit(1);
    }
    // defining variables
    int networkSocket, acceptedSocket, portno, n;
    char buffer[255];
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addr_length;

    // creating a socket
    networkSocket = socket(AF_INET, SOCK_STREAM, 0);

    // if socket creation unsuccessfull
    if (networkSocket < 0)
        error("Error opening socket.");

    // just clearing the serv addr variable
    bzero((char *)&serv_addr, sizeof(serv_addr));
    // assigning the inputed port number in the terminal arg
    portno = atoi(argv[1]);

    // server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); // host to network short

    // binding
    if (bind(networkSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
        error("Binding Failed");

    // listening ,backlog = 5
    listen(networkSocket, 5);
    client_addr_length = sizeof(client_addr);

    // accepting the incoming connection
    acceptedSocket = accept(networkSocket, (struct sockaddr *)&client_addr, &client_addr_length);
    if (acceptedSocket < 0)
        error("Failed to accept the socket");

    while (1)
    {
        // clearing our buffers or data
        bzero(buffer, sizeof(buffer));
        // reading whatever the client has sent to the server
        n = read(acceptedSocket, buffer, sizeof(buffer));

        if (n < 0)
            error("Error on reading");
        // prints whatever the client has sent
        printf("Client : %s", buffer);

        // again resets the data or buffer var
        bzero(buffer, sizeof(buffer));
        // gets the input from the server side
        fgets(buffer, 255, stdin);
        // writes the servers response
        n = write(acceptedSocket, buffer, strlen(buffer));
        if (n < 0)
            error("Error on writing");

        // get out of the loop if the server response is "bye"
        int i = strncmp("Bye", buffer, 3);
        if (i == 0)
            break;
    }

    // closing the sockets
    close(acceptedSocket);
    close(networkSocket);

    return 0;
}