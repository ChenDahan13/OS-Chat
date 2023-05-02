#include "stnc.h"


void server(char *port) {
    // Create tcp socket
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    if(sock < 1) {
        printf("Socket() failed. Error: %d\n", errno);
        exit(1);
    }
    printf("Created socket\n");

    int enableReuse = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if(ret < 0) {
        printf("Setsockopt() failed. Error: %d\n", errno);
        exit(1); 
    }
    printf("Setsockopt() success\n");

    // Create the address of the server
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; // IPV4
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(port));

    // Bind the socket to the port and ip address
    int bindres = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if(bindres < 0) {
        printf("Bind() failed. Error: %d\n", errno);
        exit(1);
    }
    printf("Bind() success\n");

    // Listen only 1 client
    int lis = listen(sock, 1);
    if(lis < 0) {
        printf("Listen() failed. Error: %d", errno);
        exit(1);
    }
    printf("Listen() success\nWaiting for TCP connections\n");

    // Accept tcp connection with client
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    int client_socket = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if(client_socket < 0) {
        printf("Accept() failed. Error: %d\n", errno);
        exit(1);
    }
    printf("Accept() success\n");
    
    // Create poll
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO; // Stdin file descriptor
    fds[0].events = POLLIN; // Alert for receiving data

    fds[1].fd = client_socket; // Stdin file descriptor
    fds[1].events = POLLIN; // Alert for receiving data

    char msg[BUFFER_SIZE]; // The message from the server

    while(1) {

        int pollres = poll(fds, 2, -1);
        if(pollres == -1) {
            printf("Poll() failed. Error: %d\n", errno);
            exit(1);
        }
        // Get data from stdin
        if(fds[0].revents & POLLIN) { 
            // Read what was written in STDIN and puts it as a message
            int bytesRead = read(fds[0].fd, msg, BUFFER_SIZE);
            if(bytesRead < 0) {
                printf("Read() failed. Error: %d\n", errno);
                exit(1);
            }
            msg[bytesRead] = "\0";

            // Send the message to the client
            int byteSend = send(client_socket, msg, bytesRead, 0);
            if(byteSend < 0) {
                printf("Send() failed. Error: %d\n", errno);
                exit(1);
            }
            memset(msg, 0, BUFFER_SIZE);
        }
        // Get data from socket
        if(fds[1].revents & POLLIN) {
            // Receive message from the client
            int bytesRecv = recv(client_socket, msg, BUFFER_SIZE, 0);
            if(bytesRecv < 0) {
                printf("Recv() failed. Error: %d\n", errno);
                exit(1);
            }
            if(bytesRecv == 0) {
                printf("Client disconnected\n");
                exit(1);
            }
            msg[bytesRecv] = "\0";
            printf("Client: %s", msg);
            memset(msg, 0, BUFFER_SIZE);
        }
    }
    close(client_socket);
    close(sock);
}

void client(char *ip, char *port) {
    // Create tcp socket
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    if(sock < 1) {
        printf("Socket() failed. Error: %d\n", errno);
        exit(1);
    }
    printf("Created socket\n");

    int enableReuse = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if(ret < 0) {
        printf("Setsockopt() failed. Error: %d\n", errno);
        exit(1); 
    }
    printf("Setsockopt() success\n");

    // Create the address of the client
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; // IPV4
    serverAddress.sin_port = htons(atoi(port));
    int rval = inet_pton(AF_INET, ip, &serverAddress.sin_addr);
    if(rval < 0) {
        printf("Inet_pton() failed. Error: %d", errno);
        exit(1);
    }
    printf("Inet_pton success\n");

    // Connect to the server
    int connected = connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if(connected < 0) {
        printf("Connect() failed. Error: %d\n", errno);
        exit(1);
    }
    printf("Connect() success\n");

    // Create poll
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO; // Stdin file descriptor
    fds[0].events = POLLIN; // Alert for receiving data

    fds[1].fd = sock; // Stdin file descriptor
    fds[1].events = POLLIN; // Alert for receiving data

    char msg[BUFFER_SIZE]; // The message from the server

    while(1) {

        int pollres = poll(fds, 2, -1);
        if(pollres == -1) {
            printf("Poll() failed. Error: %d\n", errno);
            exit(1);
        }
        // Get data from stdin
        if(fds[0].revents & POLLIN) { 
            // Read what was written in STDIN and puts it as a message
            int bytesRead = read(fds[0].fd, msg, BUFFER_SIZE);
            if(bytesRead < 0) {
                printf("Read() failed. Error: %d\n", errno);
                exit(1);
            }
            msg[bytesRead] = "\0";

            // Send the message to the server
            int byteSend = send(sock, msg, bytesRead, 0);
            if(byteSend < 0) {
                printf("Send() failed. Error: %d\n", errno);
                exit(1);
            }
            memset(msg, 0, BUFFER_SIZE);
        }
        // Get data from socket
        if(fds[1].revents & POLLIN) {
            // Receive message from the client
            int bytesRecv = recv(sock, msg, BUFFER_SIZE, 0);
            if(bytesRecv < 0) {
                printf("Recv() failed. Error: %d\n", errno);
                exit(1);
            }
            if(bytesRecv == 0) {
                printf("Server disconnected\n");
                exit(1);
            }
            msg[bytesRecv] = "\0";
            printf("Server: %s", msg);
            memset(msg, 0, BUFFER_SIZE);
        }
    }
    close(sock);
}

int main(int argc, char *argv[]) {
    // Checks number of arguments
    if(argc < 3) {
        printf("Usage: stnc -c/-s PORT/IP PORT\n");
        return 1;
    }

    if(strcmp(argv[1], "-s") == 0) {
        server(argv[2]);
        return 0;
    }
    else if(strcmp(argv[1], "-c") == 0) {
        client(argv[2], argv[3]);
        return 0;
    }
    else {
        printf("Invalid flag. try -c/-s\n");
        return 1;
    }
}
