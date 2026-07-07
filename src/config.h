#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
  char *token;
  char *key;
  bool success;
} AuthInfo;

AuthInfo *getAuthInfo();
int config(int argc, char *argv[]);

#endif
