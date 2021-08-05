#include "communication.h"
#include "client.h"

#ifndef CLIENTBOT_H
#define CLIENTBOT_H

// struct to hold the messages sent by the server
typedef struct {
    char *messageLine;
} Memory;

// struct to hold the response file data
typedef struct {
    char *stimulus;
    char *response;
} FileData;

// struct to hold all the clientbot data.
typedef struct {
    FileData *response;
    Memory *chatMessages;
    char *clientName;
    bool amVerified;
    bool myTurn;
    int storedMessages;
    int responses;
} Clientbot;

#endif
