#include "utils.h"


char *read_line(FILE *file) {
    int buffSize = BUFFSIZE;
    char *buffer = (char *) malloc(sizeof(char) * buffSize);
    int numOfReads = 0;
    int next;
    while (1) {
        next = fgetc(file);
        if (next == EOF && numOfReads == 0) {
            free(buffer);
            return NULL;
        } 
        if (numOfReads == buffSize - 1) {
            buffSize *= 2;
            buffer = (char *) realloc(buffer, sizeof(char) * buffSize);
        }
        if (next == '\n' || next == EOF) {
            buffer[numOfReads] = '\0';
            break;
        }
        buffer[numOfReads++] = next;
    }
    return buffer;
}

char *append_digit(char *name, bool digitPresent) {
    int j = 0;
    char *newName = (char *) malloc(sizeof(char) * strlen(name) + 2);
    if (!digitPresent) {
        j = sprintf(newName, "%s", name);
        j += sprintf(newName + j, "%c", '0');
        free(name);
        newName[strlen(newName)] = '\0';
    } else {
        for (int i = 0; i < strlen(name); ++i) {
            newName[i] = name[i];
            if (isdigit(name[i])) {
                char j = name[i];
                j++;
                newName[i] = j;
            }
        }
        free(name);
        newName[strlen(newName)] = '\0';
    }
    return newName;
}

char *normalise_case(char *string) {
    char *newString = (char *) malloc(sizeof(char) * strlen(string));
    for (int i = 0; i < strlen(string); ++i) {
        newString[i] = tolower(string[i]);   
    }
    return newString;
}

bool check_digit_presence(char *name) {
    bool present = false;
    for (int i = 0; i < strlen(name); ++i) {
        if (isdigit(name[i])) {
            present = true;
            return present;
        }
    }            
    return present;
}

bool check_arg_number(int argNum) {
    if (argNum > 2) {
        return false;
    }
    return true;
} 

bool check_chat_file(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        return false;
    }
    fclose(file);
    return true;
}

char *construct_chat_message(char *clientName, char *clientMessage) {
    char *message = (char *) malloc(sizeof(char) * (strlen(clientName)
            + strlen(clientMessage) + 2));
    sprintf(message, "(%s) %s", clientName, clientMessage);
    message[strlen(message)] = '\0';
    return message;
}

int get_next_client(int activeClients, int currentClient) {
    int nextClient = 0;
    if (currentClient == (activeClients - 1)) {
        nextClient = 0;
    } else if (activeClients == 1) {
        nextClient = 0;
    } else {
        nextClient = currentClient + 1;
    }
    
    return nextClient;
}

int get_slash_count(char *string) {
    int count = 0;
    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] == '/') {
            count++;
        }
    }
    return count;
} 

bool colon_present(char *line) {
    int count = 0;
    for (int i = 0; i < strlen(line); ++i) {
        if (line[i] == ':') {
            count++;
        }
    }
    if (count > 1 || count < 1) {
        return false;
    } else {
        return true;
    }
}

