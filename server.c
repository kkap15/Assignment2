#include "server.h"

/*
 * Takes in fileName as parameter and tries to open
 * the file. If it is a success  
 * return's true else false;
*/
bool check_file(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        return false;
    }
    fclose(file);
    return true;
}

/*
 * Takes in number of arguments and arguments as parameters
 * Check's if the number of arguments and arguments are valid or not
 * If any inconsistency the programs shut's down with the valid code.
*/
void check_errors(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "%s\n", "Usage: server configfile");
        fflush(stderr);
        exit(1);
    }
    if (!check_file(argv[1])) {
        fprintf(stderr, "%s\n", "Usage: server configfile");
        fflush(stderr);
        exit(1);
    }
}

/*
 * Get's the data from the config file.
 * Take's in the server struct and fileName argument to collect data
 * Store's the data in server struct.
 *
 * return's nothing.
*/
void get_data_from_file(Server *server, char *fileName) {
    FILE *configFile = fopen(fileName, "r");
    char *line, *clientPath, *chatFilePath;
    int i = 0;
    while ((line = read_line(configFile)) != NULL) {
        if (line[0] == '#') {
            continue;
        }
        if (!colon_present(line)) {
            continue;
        }
        clientPath = strtok(line, ":");
        chatFilePath = strtok(&line[strlen(clientPath) + 1], "\0");
        server->clients[i].data->clientPath = (char *) malloc(sizeof(char) * 
                strlen(clientPath));
        server->clients[i].data->chatFilePath = (char *) malloc(sizeof(char) * 
                strlen(chatFilePath));
        strcpy(server->clients[i].data->clientPath, clientPath);
        strcpy(server->clients[i].data->chatFilePath, chatFilePath);
        i++;
    }
    fclose(configFile);
}

/*
 * initialises the server with memory for it's clients and the
 * client data. Take's the server struct and number of clients as 
 * parameters.
 *
 * return's nothing.
*/
void init_server(Server *server, int numberOfClients) {
    server->numberOfClients = numberOfClients;
    server->activeClients = 0;
    server->verifiedClients = 0;
    server->clients = (ServerClients *) malloc(sizeof(ServerClients) * 
            server->numberOfClients);
    for (int i = 0; i < server->numberOfClients; ++i) {
        server->clients[i].data = (ClientData *) malloc(sizeof(ClientData) * 
                server->numberOfClients * numberOfClients);
        server->clients[i].isVerified = false;
    }
} 

/*
 * Take's in the filename and open's the file to count the number of
 * clients present.
 *
 * return's the number of clients.
*/
int get_number_of_clients(char *fileName) {
    FILE *configFile = fopen(fileName, "r");
    int count = 0;
    char *line;
    while ((line = read_line(configFile)) != NULL) {
        if (line[0] == '#') {
            continue;
        } else if (!colon_present(line)) {
            continue;
        } else {
            count++;
        }
    }   
    fclose(configFile);
    return count;
}

/*
 * Attempt's to connect to client using fork and make set's up
 * communications channels between client and itself.
 * Take's in the server and the client struct as parameters.
 * return's integer showing if the connection is successfull or not.
*/
int attempt_connection(Server *server, ServerClients *client) {
    int null = open("/dev/null", O_RDONLY);
    int forkStatus, pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);
    forkStatus = fork();
    client->pid = 0;
    if (forkStatus == -1) {
        return -1;
    } else if (forkStatus == 0) {
        close(pipe1[0]);
        close(pipe2[1]);
        if (dup2(null, 2) == -1) {
            return -1;
        }
        if (dup2(pipe2[0], 0) == -1) {
            return -1;
        }
        if (dup2(pipe1[1], 1) == -1) {
            return -1;
        }
        char *execArgs[3];
        for (int i = 0; i < 2; ++i) {
            execArgs[i] = (char *) malloc(sizeof(char ) * 125);
        }
        strcpy(execArgs[0], client->data->clientPath);
        strcpy(execArgs[1], client->data->chatFilePath);
        execArgs[2] = 0;
        if (execlp(execArgs[0], execArgs[0], execArgs[1], NULL) == -1) {
            return -1;
        }
        return 0;
    } else {
        client->pid = forkStatus;
        close(pipe2[0]);
        close(pipe1[1]);
        client->write = fdopen(pipe2[1], "w");
        client->read = fdopen(pipe1[0], "r");
        server->activeClients++;
        return 0;
    }
    return 0;
}

/*
 * Function to check the name of the client.
 * scan's all the clients for their names and if any matching name is found
 * return's bool value indicating such.
 *Takes in the server and the message struct as parameters which has the 
 * client's name stored.
*/
bool verify_name(Server *server, Message msg) {
    char *name = (char *) msg.message;
    int flag = 0;
    for (int i = 0; i < server->activeClients - 1; ++i) {
        if (strcmp(server->clients[i].clientName, name) == 0) {
            flag = 1;
            break;
        } else {
            continue;
        }
    }
    if (flag == 1) {
        return false;
    } else {
        return true;
    }
}

/*
 * Function to store name of the client 
 * Take's in the client and the name provided by the connecting client 
 * to do so.
 *
 * return's nothing
*/
void store_name(ServerClients *client, char *name) {
    client->clientName = (char *) malloc(sizeof(char) * strlen(name));
    strcpy(client->clientName, name);
}

/*
 * Print's the connecting clients name to the provided FILE* parameter.
 *
 * return's nothing.
*/
void print_client_names_to_chat(char *clientName, FILE *output) {
    fprintf(output, "(%s has entered the chat)\n", clientName);
    fflush(output);
}

/*
 * Scan's through the server struct for their names.
 * Takes the server struct in as a parameter.
 * 
 * return's nothing.
*/
void print_active_clients(Server *server) {
    for (int i = 0; i < server->activeClients; ++i) {
        char *name = server->clients[i].clientName;
        print_client_names_to_chat(name, stdout);
    }
}

/*
 * Attempt's to find the client to whom the kick message is to be sent.
 * Takes in the server struct, the message which contains the name of the
 * of the client to be kicked and the index of the client who sent the
 * the kick message.
 *
 * return's the index (an integer) of the client that has been kicked.
*/
int find_client(Server *server, char *message, int index) {
    int kickIndex = -1;
    for (int i = 0; i < server->activeClients; ++i) {
        if (i == index) {
            continue;
        }
        if (strcmp(server->clients[i].clientName, message) == 0) {
            send_kick_message_to_client(server->clients[i].write, "KICK:");
            kickIndex = i;
            break;
        }
    }
    return kickIndex;
}

/*
 * Shift's the nearest client to the index of the client that has just quit
 * or has been kicked. Subsequently shifts the other clients ahead as well.
 * Take's in the server and the index of the kicked/quit client as 
 * parameters
 *
 * return's nothing.
*/
void shift_clients(Server *server, int indexOfClient) {
    for (int i = 0; i < server->activeClients - 1; ++i) {
        if (i == indexOfClient) {
            server->clients[i] = server->clients[i + 1];
        }
    }
}

/*
 * Broadcast's the message to the clients sent by a particular client.
 * take's in the FILE * argument as the output and the name of the client that
 * has sent the message and the message sent by that client as parameters.
 *
 * return's nothing.
*/
void broadcast_message_to_client(FILE *output, char *name, char *message) {
    fprintf(output, "%s:%s:%s\n", "MSG", name, message);
    fflush(output);
}

/*
 * Start's chatting with the clients.
 *
 * Take's in the server struct as the argument.
*/
void start_server_chatting(Server *server) {
    Message msg;
    ServerClients client;
    int currentClient = 0;
    int kickIndex = 0;
    memset(&msg, 0, sizeof(Message));
    memset(&client, 0, sizeof(ServerClients));
    while (server->activeClients != 0) {
        client = server->clients[currentClient];
        send_turn_message_to_client(client.write, "YT:");
        do {
            msg = get_client_message(client.read, C_CHAT);
            if (msg.clientMessageId == C_CHAT) {
                print_client_message(client.clientName,
                        (char *) msg.message, stdout);
                for (int i = 0; i < server->activeClients; ++i) {
                    broadcast_message_to_client(server->clients[i].write,
                            client.clientName, (char *) msg.message);
                }
            } 
        } while (msg.clientMessageId == C_CHAT);
        if (msg.clientMessageId == C_DONE) {
            currentClient = get_next_client(server->activeClients, 
                    currentClient);
            continue;
        } else if (msg.clientMessageId == C_QUIT) {
            print_client_left_message(client.clientName, stdout);
            shift_clients(server, currentClient);
            server->activeClients--;
            currentClient = get_next_client(server->activeClients,
                    currentClient);
            continue;
        } else if (msg.clientMessageId == C_KICK) {
            if ((kickIndex = find_client(server, (char *) msg.message, 
                    currentClient)) >= 0) {
                shift_clients(server, kickIndex);
                server->activeClients--;
                print_client_left_message((char *) msg.message, stdout);
                continue;
            }
        }
    }
}

/*
 * Attempt's to start the server. Also tries to do name negotiations
 * with the clients.
 * Take's int the server struct as parameter. 
 * return's nothing
*/
void start_server(Server *server) {
    ServerClients client;
    memset(&client, 0, sizeof(ServerClients));
    for (int i = 0; i < server->numberOfClients; ++i) {
        client = server->clients[i];
        int clientStatus = attempt_connection(server, &client);
        if (clientStatus != 0) {
            continue;
        }
        send_who_message(client.write, "WHO:");
        Message msg = get_client_message(client.read, C_NAME);
        if (msg.clientMessageId == C_INVALID) {
            server->activeClients--;
            server->clients[i] = server->clients[i + 1];
            server->numberOfClients--;
            i--;
        } else {
            while (!verify_name(server, msg)) {
                send_name_taken_message(client.write, "NAME_TAKEN:");
                send_who_message(client.write, "WHO:");
                msg = get_client_message(client.read, C_NAME);
            } 
            store_name(&client, (char *) msg.message);
            memcpy(&server->clients[i], &client, sizeof(ServerClients));
        }
    }
}

int main(int argc, char **argv) {
    check_errors(argc, argv);
    Server server;
    memset(&server, 0, sizeof(Server));
    int numberOfClients = get_number_of_clients(argv[1]);
    init_server(&server, numberOfClients);
    struct sigaction sa = {.sa_flags = SA_RESTART};
    sigaction(SIGPIPE, &sa, NULL);
    get_data_from_file(&server, argv[1]);
    start_server(&server);
    print_active_clients(&server);
    start_server_chatting(&server);
}

