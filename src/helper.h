#ifndef HELPER_H
#define HELPER_H

typedef struct {
  bool id;
  bool recursive;
  char **argv;
  int argc;
} Flags;

Flags parseFlags(int argc, char *argv[]);

#endif
