#include <stdio.h>
#include "utils.h"

#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

// enum to figure out the server messages
typedef enum {
    S_WHO,
    S_NAME_TAKEN,
    S_YT,
    S_KICK,
    S_MSG,
    S_LEFT,
    S_UNKNOWN,
    S_INVALID
} ServerMessages;

// enum to figure out the client messages
typedef enum {
    C_NAME,
    C_CHAT,
    C_KICK,
    C_DONE,
    C_QUIT,
    C_UNKNOWN,
    C_INVALID
} ClientMessages;

// struct to hold the messages sent by srevr and clients.
typedef struct {
    void *message;
    ServerMessages serverMessageId;
    ClientMessages clientMessageId;
} Message;

// client communication function declarations.
Message get_client_message(FILE *input, ClientMessages messageType);
void send_client_name(char *name, FILE *output);
void send_chat_message(char *message, FILE *output);
void send_done_message(char *message, FILE *output);
void send_kick_message(char *message, FILE *output);
void send_quit_message(char *message, FILE *output);
void respond_to_stimuli(char *response, FILE *output);

// server communication function declarations. 
Message get_server_message(FILE *input, ServerMessages messageType);
void send_kick_message_to_client(FILE *output, char *message);
void send_name_taken_message(FILE *output, char *message);
void send_who_message(FILE *output, char *message);
void send_turn_message_to_client(FILE *output, char *message);
void print_client_left_message(char *name, FILE *output);
void print_chat_message(char *message, FILE *output);
void print_client_message(char *clientName, char *message, FILE *output);

#endif //ass2_COMMUNICATION_H 
