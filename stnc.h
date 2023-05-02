#ifndef STNC_H
#define STNC_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#define BUFFER_SIZE 1024

void server(char*);
void client(char*, char*);

#endif
