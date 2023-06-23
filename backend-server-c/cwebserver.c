#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// maximum application buffer
#define APP_MAX_BUFFER 1024
#define PORT 8080

int main()
{
    // define server and client descriptors
    int server_fd, client_fd;

    // define the socket address
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // define application buffer where we receive requests
    // data will be moved from receive buffer to here
    char buffer[APP_MAX_BUFFER] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET;         // IPV4
    address.sin_addr.s_addr = INADDR_ANY; // listen 0.0.0.0 interfaces
    address.sin_port = htons(PORT);       // listen on port 8080

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening for incoming connections...\n");

    // Creates the queues
    // Listen for incoming connection request, with 10 backlog (10 connections in accept queue)
    if (listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // infinite loop
    while (1)
    {
        printf("\nWaiting for a connection...\n");
        // Accept incoming connection client_fd -> connection
        // if the accept queue is empty, we are stuck here...
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&address_len)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // read data from the OS receive buffer to the application (buffer)
        // this is essentially reading the HTTP request
        // reading max of APP_MAX_BUFFER
        read(client_fd, buffer, APP_MAX_BUFFER);
        printf("%s\n", buffer);

        char *http_response = "HTTP/1.1 200 OK\n"
                              "Content-Type text/plain\n"
                              "Content-Length 13\n\n"
                              "Hello world";

        // write to the socket
        // send queue os
        write(client_fd, http_response, strlen(http_response));

        // close the client socket (terminate tcp connection)
        close(client_fd);
    }
    return 0;
}
