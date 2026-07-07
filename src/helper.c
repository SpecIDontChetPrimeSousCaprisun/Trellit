#include "helper.h"
#include <string.h>
#include <stdlib.h>

Flags parseFlags(int argc, char *argv[]) {
  Flags flags;
  flags.id = false;
  flags.recursive = false;
  flags.argv = malloc(argc * sizeof(char *));
  flags.argc = 0;

  for (int i = 0; i < argc; i++) {
    char *arg = argv[i];
    if (strcmp(arg, "--id") == 0 || strcmp(arg, "-i") == 0 || strcmp(arg, "--identifier") == 0) flags.id = true;
    else if (strcmp(arg, "--recursive") == 0 || strcmp(arg, "-r") == 0) flags.recursive = true;
    else {
      flags.argv[flags.argc] = arg;
      flags.argc++;
    }
  }

  return flags;
}
