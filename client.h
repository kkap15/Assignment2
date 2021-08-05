#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "communication.h"

// struct to hold client information/
typedef struct {
    char *chatFileLine;
    char *clientName;
} Client;

#endif //ass2_CLIENT_H
