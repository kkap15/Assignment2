#include "clientbot.h"

/*
 * Takes in number of arguments and arguments as parameters
 * Check's if the number of arguments and arguments are valid or not
 * If any inconsistency the programs shut's down with the valid code
 * and error message
*/
void check_errors(int argc, char **argv) {
    if (!check_arg_number(argc)) {
        fprintf(stderr, "%s\n", "Usage: clientbot responsefile");
        fflush(stderr);
        exit(1);
    }
    if (!check_chat_file(argv[1])) {
        fprintf(stderr, "%s\n", "Usage: clientbot responsefile");
        fflush(stderr);
        exit(1);
    }
}

/*
 * stores the response file data into a FileData struct.
 * take's in a line from the file as parameter.
 *
 * return's the stored filedata.
*/
FileData get_file_data(char *line) {
    FileData data;
    memset(&data, 0, sizeof(FileData));
    char *stimulus = strtok(line, ":");
    char *response = strtok(&line[strlen(stimulus) + 1], "\n");
    data.response = (char *) malloc(sizeof(char) * strlen(response));
    data.stimulus = (char *) malloc(sizeof(char) * strlen(stimulus));
    strcpy(data.response, response);
    strcpy(data.stimulus, stimulus);
    return data;
}

/*
 * Attempt's to extract the response file data and store it.
 * Takes in the fileName and the bot struct as parameters.
 * 
 * return's nothing.
*/
void get_clientbot_data(char *fileName, Clientbot *bot) {
    char *line;
    FILE *responseFile = fopen(fileName, "r");
    while ((line = read_line(responseFile)) != NULL) {
        if (line[0] == '#') {
            continue;
        } else if (!colon_present(line)) {
            continue;
        }
        if (bot->response == NULL) {
            bot->response = (FileData *) malloc(sizeof(FileData));
        } else {
            bot->response = (FileData *) realloc(bot->response, 
                    sizeof(FileData) * (bot->responses + 1));
        }
        bot->response[bot->responses] = get_file_data(line);
        bot->responses++;
    }
    fclose(responseFile);
}

/*
 * check's if the provided message is the message from itself or not.
 * Take's in the bot's name and the message sent by the server as 
 * parameters.
 *
 *  return's a bool indicating if above is true or false
*/
bool is_my_message(char *myName, char *message) {
    char *name = (char *) malloc(sizeof(char) * strlen(message));
    strcpy(name, message);
    char *token = strtok(name, ")");
    int count = 0;
    for (int i = 1, j = 0; i < strlen(token); ++i, ++j) {
        if (token[i] == myName[j]) {
            count++;
        }
    }
    if (count == strlen(myName)) {
        return true;
    }
    return false;
}

/*
 * extract's the chat messsage.
 * take's in the message constructed by itself as parameter.
 *
 * return's the chat message.
*/
char *get_chat_message(char *message) {
    int index = 0;
    for (int i = 0; i < strlen(message); ++i) {
        if (isspace(message[i])) {
            index = i;
            break;
        }
    }
    index++;
    char *chat = (char *) malloc(sizeof(char) * BUFFSIZE);
    int j = 0;
    while (message[index] != '\0') {
        chat[j] = message[index];
        index++;
        j++;
    }
    return chat;
}

/*
 * Store's the message sent by the client in a struct.
 *  Take's the message and it's name as parameters.
 * return's a Memory struct after storing the message
*/
Memory store_message(char *message, char *myName) {
    Memory storage;
    memset(&storage, 0, sizeof(Memory));
    storage.messageLine = (char *) malloc(sizeof(char) * strlen(message));
    if (is_my_message(myName, message)) {
        storage.messageLine = NULL;
        return storage;
    }
    char *chat = get_chat_message(message);
    strcpy(storage.messageLine, chat);
    return storage;
}

/*
 * search's the message for a stimulus.
 * Takes in the message and the stimulus as the arguments.
 * return's a bool indicationg if stimulus found or not.
*/
bool search_pattern(char *message, char *stimulus) {
    int n = 0;
    int m = 0;
    int len = strlen(stimulus);
    while (message[n] != '\0') {
        if (message[n] == stimulus[m]) {
            while (message[n] == stimulus[m] && message[n] != '\0') {
                n++;
                m++;
            }
            if (m == len) {
                return true;
            }
        } else {
            while (message[n] != ' ') {
                n++;
                if (message[n] == '\0') {
                    break;
                }
            }
        }
        n++;
        m = 0;
    }
    return false;
}

/*
 * scan's the saved message for stimulus.
 * takes in the bot struct and the number signifying the messages stored
 * as parameters.
 *
 * return's nothing.
*/
void scan_saved_messages_for_pattern(Clientbot *bot, int index) {
    for (int i = 0; i < index; ++i) {
        char *message = bot->chatMessages[i].messageLine;
        if (message == NULL) {
            continue;
        }
        message = normalise_case(message);
        for (int j = 0; j < bot->responses; ++j) {
            char *stimulus = bot->response[j].stimulus;
            stimulus = normalise_case(stimulus);
            if (search_pattern(message, stimulus)) {
                respond_to_stimuli(bot->response[j].response, stdout);
                break;
            }
        }
    }
}

/*
 * the bot start's chatting after recieving YT: froim server.
 *
 * return's nothing
*/
void start_chatting(Clientbot *bot) {
    while (1) {
        if (bot->chatMessages == NULL) {
            break;
        } else {
            scan_saved_messages_for_pattern(bot, bot->storedMessages);
            bot->storedMessages = 0;
            break;
        }
    }
}

/*
 * allocates memory to the memory of the bot to 
 * store messages.
 * return's nothing.
*/
void allocate_memory(Clientbot *bot) {
    if (bot->chatMessages == NULL) {
        bot->chatMessages = (Memory *) malloc(sizeof(Memory));
    } else {
        bot->chatMessages = (Memory *) realloc(bot->chatMessages, 
                sizeof(Memory) * (bot->storedMessages + 1));
    }
}

/*
 * start's the clientbot and takes fileName and bot as 
 * parameters.
 * return's nothing and run's till it recieves KICK: from
 * server.
*/
void start_clientbot(char *fileName, Clientbot *bot) {
    Message msg;
    memset(&msg, 0, sizeof(Message));
    bool isDigitPresent;
    while (1) {
        msg = get_server_message(stdin, S_WHO);
        switch (msg.serverMessageId) {
            case S_WHO:
                send_client_name(bot->clientName, stdout);
                break;
            case S_NAME_TAKEN:
                isDigitPresent = check_digit_presence(bot->clientName);
                bot->clientName = append_digit(bot->clientName, 
                    isDigitPresent);
                break;
            case S_MSG:
                allocate_memory(bot);
                print_chat_message((char *) msg.message, stderr);
                bot->chatMessages[bot->storedMessages++] = 
                        store_message((char *) msg.message, bot->clientName);
                break;
            case S_YT:
                bot->amVerified = true;
                bot->myTurn = true;
                break;
            case S_KICK:
                fprintf(stderr, "%s\n", "Kicked");
                fflush(stderr);
                exit(3);
                break;
            case S_LEFT:
                print_client_left_message((char *) msg.message, stderr);
                break;
            case S_INVALID:
            case S_UNKNOWN:
                fprintf(stderr, "%s\n", "Communications error");
                fflush(stderr);
                exit(2);
        }
        if (bot->amVerified && bot->myTurn) {
            start_chatting(bot);
            bot->myTurn = false;
            send_done_message("DONE:", stdout);
            bot->chatMessages = NULL;
        }
    }
}

/*
 * set's the name of the clientbot after reading it from the file 
 * takes in the bot and fileName as parameters.
 * return's nothing.
*/
void set_clientbot_name(Clientbot *bot, char *name) {
    bot->clientName = (char *) malloc(sizeof(char) * BUFFSIZE);
    int countSlash = 0;
    int j = 0;
    countSlash = get_slash_count(name);
    for (int i = 0; i < strlen(name); ++i) {
        if (name[i] == '/') {
            countSlash--;
        }
        if (countSlash == 0) {
            strcpy(&bot->clientName[j], &name[i + 1]);
            j++;
        }
    }
}

/*
 * initialises client bot.
 * return's nothing.
*/
void init_client_bot(Clientbot *bot) {
    bot->amVerified = false;
    bot->myTurn = false;
    bot->storedMessages = 0;
    bot->responses = 0;
    bot->response = NULL;
    bot->chatMessages = NULL;
}

int main(int argc, char **argv) {
    Clientbot bot;
    memset(&bot, 0, sizeof(Clientbot));
    check_errors(argc, argv);
    set_clientbot_name(&bot, argv[0]);
    init_client_bot(&bot);
    get_clientbot_data(argv[1], &bot);
    start_clientbot(argv[1], &bot);
}

