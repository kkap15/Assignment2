#include "utils.h"
#include "communication.h"
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


#ifndef SERVER_H
#define SERVER_H

// struct to hold the client data from the configfile
typedef struct {
    char *clientPath;
    char *chatFilePath;
} ClientData;

// struct to hold client data like name and their fds
typedef struct {
    pid_t pid;
    ClientData *data;
    char *clientName;
    FILE *read;
    FILE *write;
    bool isVerified;
} ServerClients;

// struct to hold server information.
typedef struct {
    int numberOfClients;
    int activeClients;
    int verifiedClients;
    ServerClients *clients;
} Server;

#endif
