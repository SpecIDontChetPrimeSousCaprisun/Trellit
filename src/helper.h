#ifndef HELPER_H
#define HELPER_H

typedef struct {
  bool id;
  bool recursive;
  bool hasPath;
  char **argv;
  char *path;
  char *currentList;
  int argc;
} Flags;

Flags parseFlags(int argc, char *argv[]);

#endif
