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

int overwrite_line(const char *path, const char *old_line, const char *new_line);
Flags parseFlags(int argc, char *argv[]);

#endif
