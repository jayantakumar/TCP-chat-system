#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // contains constants and structures needed for domain addrs and other such stuff
#include <unistd.h>
#include <netdb.h>

/*
Terminal format to use
filename server_ipaddr portno
*/

void error(const char *msg)
{
    perror(msg); // uses stderr
    exit(0);
}

int main(int argc, char *argv[])
{
    int networkSocket, acceptedSocket, portno, n;
    char buffer[255];
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    networkSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (networkSocket < 0)
    {
        error("Error opening socket.");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error no such host");
    }

    // setting up the inputted server address
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // connecting to the  server
    if (connect(networkSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Connection to server failed");

    while (1)
    {
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        n = write(networkSocket, buffer, strlen(buffer));
        ;
        if (n < 0)
            error("Error on writing");
        bzero(buffer, sizeof(buffer));
        n = read(networkSocket, buffer, sizeof(buffer));
        if (n < 0)
            error("Error on reading");
        printf("Server: %s", buffer);

        // get out of the loop if the server response is "bye"
        int i = strncmp("Bye", buffer, 3);
        if (i == 0)
            break;
    }
    close(networkSocket);
    return 0;
}