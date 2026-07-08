#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "helper.h"

static char *getTarget(char *level) {
  const char *home = getenv("HOME");
  if (home == NULL) {
    fprintf(stderr, "No home directory\n");
    return "";
  }

  char *target;

  if (strcmp(level, "global") == 0) {
    target = malloc(strlen(home) + strlen("/.trellitconfig") + 1); // +1 for '\0'
    snprintf(target, strlen(home) + strlen("/.trellitconfig") + 1, "%s%s", home, "/.trellitconfig");
  } else if (strcmp(level, "local") == 0) {
    target = malloc(strlen(".trellit/config") + 1);
    strcpy(target, ".trellit/config");
  } else {
    fprintf(stderr, "Unknown level : %s\n", level);
    return "";
  }

  return target;
}

static char *getConfigValue(char *level, char *key) {
  char *target = getTarget(level);

  if (strcmp(target, "") == 0) return "";

  FILE *f = fopen(target, "r");
  if (f == NULL) {
    perror("fopen");
    return "";
  }

  char *value = "";
  char line[256];

  while (fgets(line, sizeof(line), f) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    char lKey[128], lValue[128];
    if (sscanf(line, "%127[^=]=%127[^\n]", lKey, lValue) == 2) {
      if (strcmp(lKey, key) == 0) value = strdup(lValue);
    }
  }

  free(target);
  return value;
}

int config(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "The config subcommand requires 3 arguments !\n");
    fprintf(stderr, "Run trellit help for more details\n");
    return 1;
  }

  char *target = getTarget(argv[2]);

  if (strcmp(target, "") == 0) return 1;

  char *oldValue = getConfigValue(argv[2], argv[3]);

  if (strcmp(oldValue, "") != 0) {
    char old_line[256];
    char new_line[256];

    snprintf(old_line, sizeof(old_line), "%s=%s", argv[3], oldValue);
    snprintf(new_line, sizeof(new_line), "%s=%s", argv[3], argv[4]);

    int r = overwrite_line(target, old_line, new_line);

    free(oldValue);
    return r;
  } else {
    FILE *f = fopen(target, "a");
    if (f == NULL) {
      perror("fopen");
      return 1;
    }

    fprintf(f, "%s=%s\n", argv[3], argv[4]);

    fclose(f);
    free(target);
    return 0;
  }
}

AuthInfo *getAuthInfo() {
  AuthInfo *info = malloc(sizeof(AuthInfo));
  
  info->success = true;

  char *key = getConfigValue("local", "key");
  char *token = getConfigValue("local", "token");

  if (strcmp(key, "") == 0) key = getConfigValue("global", "key");
  if (strcmp(key, "") == 0) info->success = false;

  if (strcmp(token, "") == 0) token = getConfigValue("global", "token");
  if (strcmp(token, "") == 0) info->success = false;

  info->key = key;
  info->token = token;

  return info;
}

char *getId(char *path) {
  size_t size = strlen(path) + strlen("/info") + 1;
  char *infoPath = malloc(size);
  snprintf(infoPath, size, "%s/info", path);

  FILE *f = fopen(infoPath, "r");
  if (f == NULL) {
    perror("fopen");
    return "";
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

  fclose(f);

  return value;
}
