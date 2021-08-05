#include <stdio.h>
#include "client.h"

/*
 * Takes in number of arguments and arguments as parameters
 * Check's if the number of arguments and arguments are valid or not
 * If any inconsistency the programs shut's down with the valid code
 * and error message
*/
void check_errors(int argNum, char **args) {
    if (!check_arg_number(argNum)) {
        fprintf(stderr, "%s\n", "Usage: client chatscript");
        fflush(stderr);
        exit(1);
    }
    
    if (!check_chat_file(args[1])) {
        fprintf(stderr, "%s\n", "Usage: client chatscript");
        fflush(stderr);
        exit(1);
    }
}

/*
 * shut's down the client.
 * Take's in client struct and the chatfile as parameters.
 * return's nothing
*/
void shut_down_client(Client *client, FILE *script) {
    free(client->clientName);
    client = 0;
    fclose(script);
    exit(0);
}

/*
 * After YT: the client starts sending messages from the script.
 * Take's in the filename and client struct as parameters.
 * 
 * return's nothing
*/
void start_chatting(FILE *fileName, Client *client) {
    char *line;
    while (1) {
        line = read_line(fileName);
        if (line == NULL) {
            break;
        } 
        if (!colon_present(line)) {
            continue;
        }
        switch (line[0]) {
            case 'C': 
                if (strncmp(line, "CHAT:", 5) == 0) {
                    send_chat_message(line, stdout);
                }
                break;
            case 'D':
                if (strncmp(line, "DONE:", 5) == 0) {
                    send_done_message(line, stdout);
                }
                break;
            case 'K':
                if (strncmp(line, "KICK:", 5) == 0) {
                    send_kick_message(line, stdout);
                }
                break;
            case 'Q':
                if (strcmp(line, "QUIT:") == 0) {
                    send_quit_message(line, stdout);
                    shut_down_client(client, fileName);
                }
                break;
        }
        if (strcmp(line, "DONE:") == 0) {
            break;
        }
    }
}

/*
 * Start's the client.
 * takes in the name of the file (a string) and the client struct
 * as arguments.
 *
 * return's nothing. 
*/
void start_client(char *fileName, Client *client) {
    Message message;
    bool isDigitPresent;
    FILE *script = fopen(fileName, "r");
    memset(&message, 0, sizeof(Message));
    while (1) {
        message = get_server_message(stdin, S_WHO);
        switch (message.serverMessageId) {
            case S_WHO:
                send_client_name(client->clientName, stdout);
                break;
            case S_NAME_TAKEN:
                isDigitPresent = check_digit_presence(client->clientName);
                client->clientName = append_digit(client->clientName, 
                    isDigitPresent);
                break;
            case S_YT:
                start_chatting(script, client);
                break;
            case S_KICK:
                fprintf(stderr, "%s\n", "Kicked");
                fflush(stderr);
                exit(3);
            case S_LEFT:
                print_client_left_message(message.message, stderr);
                break;
            case S_MSG:
                print_chat_message((char *) message.message, stderr);
                break;
            case S_INVALID:
            case S_UNKNOWN: 
                fprintf(stderr, "%s\n", "Communications error");
                fflush(stderr);
                exit(2);
        }
    }
}

/*
 * set's the name of the client after reading it from the file 
 * takes in the client and fileName as parameters.
 * return's nothing.
*/
void set_client_name(Client *client, char *name) {
    client->clientName = (char *) malloc(sizeof(char ) * BUFFSIZE);
    int countSlash = 0;
    int j = 0;
    countSlash = get_slash_count(name);
    for (int i = 0; i < strlen(name); ++i) {
        if (name[i] == '/') {
            countSlash--;
        }
        if (countSlash == 0) {
            strcpy(&client->clientName[j], &name[i + 1]);
            j++;
        }
    }
}

int main(int argc, char **argv) {
    Client client;
    memset(&client, 0, sizeof(Client));
    check_errors(argc, argv);
    set_client_name(&client, argv[0]);
    start_client(argv[1], &client);
}

