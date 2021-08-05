#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define BUFFSIZE 80

#ifndef UTILS_H
#define UTILS_H

// utility functions' declarations.
bool colon_present(char *line);
bool check_digit_presence(char *name);
bool check_arg_number(int argNum);
bool check_chat_file(char *fileName);

char *append_digit(char *name, bool digitPresent);
char *normalise_case(char *string);
char *read_line(FILE *file);
char *construct_chat_message(char *clientName, char *chatMessage);

int get_next_client(int activeClients, int currentClient);
int get_slash_count(char *string);


#endif //ass2_UTILS_H
