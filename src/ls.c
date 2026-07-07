#include "ls.h"
#include "helper.h"

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

static int listCards(char *list, bool id) {
  size_t size = strlen(list) + strlen(".trellit/board/") + 1;
  char *path = malloc(size);
  snprintf(path, size, ".trellit/board/%s", list);

  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    
    size_t fullSize = strlen("/") + strlen(entry->d_name) + strlen(path) + 1;
    char* fullPath = malloc(fullSize);
    snprintf(fullPath, fullSize, "%s/%s", path, entry->d_name);

    FILE* f = fopen(fullPath, "r");
    if (f == NULL) {
        perror("fopen");
        continue;
    }

    char *value = "";
    char line[256];

    while (fgets(line, sizeof(line), f) != NULL) {
      line[strcspn(line, "\n")] = '\0';
      char lKey[128], lValue[128];
      if (sscanf(line, "%127[^=]=%127[^\n]", lKey, lValue) == 2) {
        if (strcmp(lKey, "id") == 0) value = strdup(lValue);
      }
    }

    printf("%s (%s) CARD\n", entry->d_name, value);

    free(fullPath);
  }

  closedir(dir);
  free(path);
  return 0;
}

static int listLists(bool recursive) {
  DIR *dir = opendir(".trellit/board");
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  bool first = true;
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

    char *value = "";
    char line[256];

    while (fgets(line, sizeof(line), f) != NULL) {
      line[strcspn(line, "\n")] = '\0';
      char lKey[128], lValue[128];
      if (sscanf(line, "%127[^=]=%127[^\n]", lKey, lValue) == 2) {
        if (strcmp(lKey, "id") == 0) value = strdup(lValue);
      }
    }

    size_t printSize = strlen(value) + strlen(entry->d_name) + strlen(" () LIST");

    if (recursive && first) {
      for (int i = 0; i < printSize; i++) {
        printf("=");
      }

      printf("\n");
      first = false;
    }

    printf("%s (%s) LIST\n", entry->d_name, value);

    free(path);

    if (recursive) {
      for (int i = 0; i < printSize; i++) {
        printf("-");
      }

      printf("\n");
      listCards(entry->d_name, false);
      for (int i = 0; i < printSize; i++) {
        printf("=");
      }

      printf("\n");
    }
  }

  closedir(dir);
  return 0;
}

int ls(int argc, char *argv[]) {
  Flags flags = parseFlags(argc, argv);

  if (flags.argc > 4) {
    printf("The ls subcommand only accepts up to 2 arguments\n");
    fprintf(stderr, "Run trellit help for more details\n");
    return 1;
  }
  
  if (flags.argc == 2) return listLists(flags.recursive);
  else return listCards(argv[2], flags.id);

  return 0;
}
