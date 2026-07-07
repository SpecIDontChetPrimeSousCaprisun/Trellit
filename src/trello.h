#ifndef TRELLO_H
#define TRELLO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t size;
} ResponseBuffer;

char *trello_get_cards(char *id);
char *trello_get_workspaces();
char *trello_get_boards(char *workspaceId);
char *trello_get_lists(char *id);
char *trello_get_cards_from_list(char *id);

#endif
