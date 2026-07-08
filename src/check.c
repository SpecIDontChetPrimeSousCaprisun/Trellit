#include "check.h"
#include "helper.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int check(int argc, char *argv[]) {
  Flags flags = parseFlags(argc, argv);

  if (flags.argc < 4) {
    fprintf(stderr, "The check subcommand requires 2 arguments !\n");
    fprintf(stderr, "Run trellit help for more details\n");
    return 1;
  }

  char* basePath;
  
  if (flags.hasPath) basePath = strdup(flags.path);
  else basePath = strdup(".trellit/board");
  
  size_t size = strlen(basePath) + strlen("//") + strlen(argv[2]) + strlen(argv[3]) + 1;
  char* path = malloc(size);
  snprintf(path, size, "%s/%s/%s", basePath, argv[2], argv[3]);

  overwrite_line(path, "dueComplete=false", "dueComplete=true");
  free(basePath);
}
