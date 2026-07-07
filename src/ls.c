#include "ls.h"
#include "helper.h"

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

static char *getListFromId(char *id) {
  DIR *dir = opendir(".trellit/board");
  if (dir == NULL) {
    perror("opendir");
    return "";
  }

  char *name = NULL;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    
    size_t size = strlen(".trellit/board//info") + strlen(entry->d_name) + 1;
    char* path = malloc(size);
    snprintf(path, size, ".trellit/board/%s/info", entry->d_name);

    FILE* f = fopen(path, "r");
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

    if (value != NULL && strcmp(value, id) == 0) {
      name = strdup(entry->d_name);
      free(value);
      break;
    }
    free(value);
    fclose(f);
    free(path);
  }

  closedir(dir);
  return name;
}

static int listCards(Flags flags) {
  char *listId = flags.argv[2];
  bool id = flags.id;

  char *list = NULL;

  if (id) list = getListFromId(listId);
  else if (flags.recursive) list = strdup(flags.currentList);
  else list = strdup(listId);

  char* basePath;
  
  if (flags.hasPath) basePath = strdup(flags.path);
  else basePath = strdup(".trellit/board");

  size_t size = strlen(list) + strlen(basePath) + strlen("/") + 1;
  char *path = malloc(size);
  snprintf(path, size, "%s/%s", basePath, list);

  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  // Pass 1: count real entries (skip . and ..)
  int count = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
      count++;
  }

  // Rewind to the start of the directory stream to read it again
  rewinddir(dir);

  // Now we know exactly how many slots we need
  char **cards = malloc(count * sizeof(char *));

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "info") == 0) continue;
    
    size_t fullSize = strlen("/") + strlen(entry->d_name) + strlen(path) + 1;
    char* fullPath = malloc(fullSize);
    snprintf(fullPath, fullSize, "%s/%s", path, entry->d_name);

    FILE* f = fopen(fullPath, "r");
    if (f == NULL) {
        perror("fopen");
        continue;
    }

    char *value = NULL;
    int order = 0;
    char line[256];

    while (fgets(line, sizeof(line), f) != NULL) {
      line[strcspn(line, "\n")] = '\0';
      char lKey[128], lValue[128];
      if (sscanf(line, "%127[^=]=%127[^\n]", lKey, lValue) == 2) {
        if (strcmp(lKey, "id") == 0) value = strdup(lValue);
        else if (strcmp(lKey, "order") == 0) order = atoi(lValue);
      }
    }

    char *card;

    if (flags.recursive) {
      size_t cardSize = strlen("    ()\n") + strlen(entry->d_name) + strlen(value) + 1;
      card = malloc(cardSize);
      snprintf(card, cardSize, "   %s (%s)\n", entry->d_name, value);
    } else {
      size_t cardSize = strlen(" ()\n") + strlen(entry->d_name) + strlen(value) + 1;
      card = malloc(cardSize);
      snprintf(card, cardSize, "%s (%s)\n", entry->d_name, value);
    }

    cards[order] = card;

    free(fullPath);
    free(value);
    fclose(f);
  }

  for (int i = 0; i < count; i++) {
    printf(cards[i]);
    free(cards[i]);
  }

  closedir(dir);
  free(path);
  free(list);
  return 0;
}

static int listLists(Flags flags) {
  char* path;
  
  if (flags.hasPath) path = strdup(flags.path);
  else path = strdup(".trellit/board");

  DIR *dir = opendir(path);

  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    
    size_t size = strlen(path) + strlen("//info") + strlen(entry->d_name) + 1;
    char* listPath = malloc(size);
    snprintf(listPath, size, "%s/%s/info", path, entry->d_name);

    FILE* f = fopen(listPath, "r");
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

    printf("%s (%s)\n", entry->d_name, value);

    free(listPath);
    free(value);
    fclose(f);

    if (flags.recursive) {
      Flags newFlags = flags;
      newFlags.currentList = entry->d_name;
      listCards(newFlags);
      printf("\n");
    }
  }

  closedir(dir);
  free(path);
  return 0;
}

int ls(int argc, char *argv[]) {
  Flags flags = parseFlags(argc, argv);

  if (flags.argc > 4) {
    printf("The ls subcommand only accepts up to 2 arguments\n");
    fprintf(stderr, "Run trellit help for more details\n");
    return 1;
  }
  
  if (flags.argc == 2) return listLists(flags);
  else return listCards(flags);

  return 0;
}
