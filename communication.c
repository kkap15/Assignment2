#include "communication.h"

/*
 * parses who message from server.
 * return's a message if parse successful
*/
Message get_who_message_from_server(FILE *input) {
    Message message;
    char *line = read_line(input);
    memset(&message, 0, sizeof(Message));
    if (strncmp(line, "HO:", strlen("HO:")) != 0) {
        message.message = (void *) 0;
        message.serverMessageId = S_INVALID;
        free(line);
        return message;
    }
    message.message = (char *) "WHO:";
    message.serverMessageId = S_WHO;
    free(line);
    return message;
}

/*
 * parses the name taken message
 * return's a message struct.
*/
Message get_name_taken_message_from_server(FILE *input) {
    Message message;
    char *line = read_line(input);
    if (strcmp(line, "AME_TAKEN:") == 0) {
        message.message = (char *) "NAME_TAKEN:";
        message.serverMessageId = S_NAME_TAKEN;
        free(line);
        return message;
    }
    message.message = (void *) 0;
    message.serverMessageId = S_INVALID;
    return message;
}

/*
 * parses the turn message from server
 * return's a message struct if success or not.
*/
Message get_turn_message_from_server(FILE *input) {
    Message message;
    char *line = read_line(input);
    if (strcmp(line, "T:") == 0) {
        message.message = (char *) "YT:";
        message.serverMessageId = S_YT;
        free(line);
        return message;
    }
    message.message = (void *) 0;
    message.serverMessageId = S_INVALID;
    free(line);
    return message;
}

// parses the MSG: from the server.
Message process_message_from_server(FILE *input) {
    Message message;
    char *line = read_line(input);
    char *chatLine = strtok(line, ":");
    if (strcmp(chatLine, "SG") != 0) {
        message.message = (void *) 0;
        message.serverMessageId = S_INVALID;
        return message;
    }
    char *clientNameToken = strtok(&chatLine[strlen(chatLine) + 1], ":");
    if (clientNameToken == NULL) {
        message.message = (void *) 0;
        message.serverMessageId = S_INVALID;
        return message;
    }
    char *chat = strtok(&chatLine[strlen(chatLine) + 
            strlen(clientNameToken)] + 2, "\0");
    //printf("%d %s\n", check, chatLine);
    if (chat == NULL) {
        message.message = (void *) 0;
        message.serverMessageId = S_INVALID;
        return message;
    }
    message.serverMessageId = S_MSG;
    message.message = construct_chat_message(clientNameToken, chat);
    //message.message = (void *) 1;
    //print_client_message(clientNameToken, chat, stderr);
    return message;
}

// 
Message get_name_message_from_client(FILE *input) {
    Message msg;
    char *line = read_line(input);
    memset(&msg, 0, sizeof(Message));
    char *message = strtok(line, ":");
    char *name = strtok(&line[strlen(message) + 1], "\0");
    msg.message = (char *) name;
    msg.clientMessageId = C_NAME;
    free(line);
    return msg;
}

Message get_chat_message_from_client(FILE *input) {
    Message msg;
    memset(&msg, 0, sizeof(Message));
    char *line = read_line(input);
    char *placeholder = strtok(line, ":");
    if (strncmp(placeholder, "HAT", strlen("HAT"))) {
        msg.message = (void *) 0;
        msg.clientMessageId = C_QUIT;
        return msg;
    }
    char *message = strtok(&line[strlen(placeholder) + 1], "\0");
    if (message == NULL) {
        msg.message = (void *) 0;
        msg.clientMessageId = C_QUIT;
        return msg;
    }
    msg.message = (char *) message;
    msg.clientMessageId = C_CHAT;
    free(line);
    return msg;
}

Message get_done_message_from_client(FILE *input) {
    Message msg;
    memset(&msg, 0, sizeof(Message));
    char *line = read_line(input);
    if (!strcmp(line, "ONE:")) {
        msg.message = (char *) "DONE:";
        msg.clientMessageId = C_DONE;
        return msg;
    }
    msg.message = (void *) 0;
    msg.clientMessageId = C_INVALID;
    return msg;
}

Message get_quit_message_from_client(FILE *input) {
    Message msg;
    char *line = read_line(input);
    if (strcmp(line, "UIT:") == 0) {
        msg.message = (char *) "QUIT:";
        msg.clientMessageId = C_QUIT;
        return msg;
    }
    msg.clientMessageId = C_UNKNOWN;
    msg.message = (void *) 0;
    return msg;
}

Message get_kick_message_from_client(FILE *input) {
    Message msg;
    char *line = read_line(input);
    char *message = strtok(line, ":");
    char *clientName = strtok(&line[strlen(message) + 1], "\0");
    if (strcmp(message, "ICK") != 0) {
        msg.message = (void *) 0;
        msg.clientMessageId = C_UNKNOWN;
        return msg;
    }
    if (clientName == NULL) {
        msg.message = (void *) 0;
        msg.clientMessageId = C_UNKNOWN;
        return msg;
    }
    msg.message = clientName;
    msg.clientMessageId = C_KICK;
    return msg;
}

Message get_kick_message_from_server(FILE *input) {
    Message msg;
    char *line = read_line(input);
    if (strncmp(line, "ICK:", strlen("ICK:")) != 0) {
        msg.serverMessageId = S_INVALID;
        msg.message = (void *) 0;
        return msg;
    }
    msg.serverMessageId = S_KICK;
    msg.message = (void *) 1;
    return msg;
}

Message get_left_message_from_server(FILE *input) {
    Message msg;
    char *line = read_line(input);
    if (strncmp(line, "EFT:", strlen("EFT:")) != 0) {
        msg.serverMessageId = S_INVALID;
        msg.message = (void *) 0;
        free(line);
        return msg;
    }
    char *clientName = strtok(&line[strlen("EFT:")], "\0");
    if (strlen(clientName) <= 0) {
        msg.serverMessageId = S_INVALID;
        msg.message = (void *) 0;
        free(line);
        return msg;
    }
    msg.serverMessageId = S_LEFT;
    msg.message = clientName;
    return msg; 
}

Message get_server_message(FILE *input, ServerMessages messageType) {
    Message msg;
    memset(&msg, 0, sizeof(Message));
    msg.serverMessageId = S_UNKNOWN;
    msg.message = (void *) 0;
    char first = fgetc(input);
    switch (first) {
        case 'W':
            msg = get_who_message_from_server(input);
            break;
        case 'N':
            msg = get_name_taken_message_from_server(input);
            break;
        case 'Y':
            msg = get_turn_message_from_server(input);
            break;
        case 'L':
            msg = get_left_message_from_server(input);
            break;
        case 'K':
            msg = get_kick_message_from_server(input);
            break;
        case 'M':
            msg = process_message_from_server(input);
        default:
            break;
    }   
    return msg;
}

Message get_client_message(FILE *input, ClientMessages messageId) {
    Message msg;
    memset(&msg, 0, sizeof(Message));
    msg.clientMessageId = C_UNKNOWN;
    msg.message = (void *) 0;
    char first = fgetc(input);
    //fprintf(stdout, "%c\n", first);
    //fflush(stdout);
    switch (first) {
        case 'N':
            msg = get_name_message_from_client(input);
            break;
        case 'C':
            msg = get_chat_message_from_client(input);
            break;
        case 'D':
            msg = get_done_message_from_client(input);
            break;
        case 'Q':
            msg = get_quit_message_from_client(input);
            break;
        case 'K':
            msg = get_kick_message_from_client(input);
            break;
        default:
            //printf("ihjfgd\n");
            msg.message = (void *) 0;
            msg.clientMessageId = C_INVALID;
            break;    
    }
    return msg;
}

void send_client_name(char *name, FILE *output) {
    fprintf(output, "%s%s\n", "NAME:", name);
    fflush(output);
}

void send_chat_message(char *message, FILE *output) {
    fprintf(output, "%s\n", message);
    fflush(output);
} 

void send_done_message(char *message, FILE *output) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void send_kick_message(char *message, FILE *output) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void send_quit_message(char *message, FILE *output) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void respond_to_stimuli(char *response, FILE *output) {
    fprintf(output, "%s%s\n", "CHAT:", response);
    fflush(output);
}

void send_turn_message_to_client(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
} 

void send_who_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void send_name_taken_message(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void send_kick_message_to_client(FILE *output, char *message) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void print_chat_message(char *message, FILE *output) {
    fprintf(output, "%s\n", message);
    fflush(output);
}

void print_client_message(char *clientName, char *message, FILE *output) {
    fprintf(output, "(%s) %s\n", clientName, message);
    fflush(output);
}

void print_client_left_message(char *name, FILE *output) {
    fprintf(output, "(%s %s)\n", name, "has left the chat");
    fflush(output);
}

