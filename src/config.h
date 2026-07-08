#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
  char *token;
  char *key;
  bool success;
} AuthInfo;

AuthInfo *getAuthInfo();
char *getId(char *path);
int config(int argc, char *argv[]);

#endif
