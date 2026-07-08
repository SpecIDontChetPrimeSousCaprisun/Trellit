#include "push.h"
#include "helper.h"
#include "trello.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

static int pushCards(char *path) {
  DIR *dir = opendir(path);

  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "info") == 0) continue;
    size_t size = strlen(path) + strlen(entry->d_name) + strlen("/") + 1;
    char *cardPath = malloc(size);
    snprintf(cardPath, size, "%s/%s", path, entry->d_name);

    trello_update_card(cardPath);
  }

  closedir(dir);
}

int push(int argc, char *argv[]) {
  Flags flags = parseFlags(argc, argv);

  char *path;

  if (flags.hasPath) path = strdup(flags.path);
  else path = strdup(".trellit/board");

  DIR *dir = opendir(path);

  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "info") == 0) continue;
    size_t size = strlen(path) + strlen(entry->d_name) + strlen("//info") + 1;
    char *infoPath = malloc(size);
    snprintf(infoPath, size, "%s/%s/info", path, entry->d_name);

    FILE* f = fopen(infoPath, "r");
    if (f == NULL) {
        perror("fopen");
        continue;
    }

    char *value = NULL;
    char line[256];

    while (fgets(line, sizeof(line), f) != NULL) {
      line[strcspn(line, "\n")] = '\0';
      char lKey[128], lValue[128];
      if (sscanf(line, "%127[^=]=%127[^\n]", lKey, lValue) == 2) {
        if (strcmp(lKey, "id") == 0) value = strdup(lValue);
      }
    }

    if (value == NULL) {
      trello_create_list(entry->d_name, path);
    }

    size_t listSize = strlen(path) + strlen(entry->d_name) + strlen("/") + 1;
    char *listPath = malloc(listSize);
    snprintf(listPath, listSize, "%s/%s", path, entry->d_name);

    pushCards(listPath);

    free(infoPath);
    free(listPath);
    free(value);
    fclose(f);
  }

  closedir(dir); 
  free(path);
  return 0;
}
