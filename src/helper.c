#include "helper.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Flags parseFlags(int argc, char *argv[]) {
  Flags flags;
  flags.id = false;
  flags.recursive = false;
  flags.hasPath = false;
  flags.argv = malloc(argc * sizeof(char *));
  flags.argc = 0;

  bool nextPath = false;

  for (int i = 0; i < argc; i++) {
    char *arg = argv[i];
    if (nextPath) {
      flags.path = arg;
      nextPath = false;
    } else if (strcmp(arg, "--id") == 0 || strcmp(arg, "-i") == 0 || strcmp(arg, "--identifier") == 0) flags.id = true;
    else if (strcmp(arg, "--recursive") == 0 || strcmp(arg, "-r") == 0) flags.recursive = true;
    else if (strcmp(arg, "--path") == 0 || strcmp(arg, "-p") == 0) {
      nextPath = true;
      flags.hasPath = true;
    } else {
      flags.argv[flags.argc] = arg;
      flags.argc++;
    }
  }

  return flags;
}

int overwrite_line(const char *path, const char *old_line, const char *new_line) {
  FILE *in = fopen(path, "r");
  if (in == NULL) {
      perror("fopen");
      return 1;
  }

  char tmp_path[512];
  snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

  FILE *out = fopen(tmp_path, "w");
  if (out == NULL) {
      perror("fopen");
      fclose(in);
      return 1;
  }

  char line[256];
  while (fgets(line, sizeof(line), in) != NULL) {
      char trimmed[256];
      strcpy(trimmed, line);
      trimmed[strcspn(trimmed, "\n")] = '\0';  // strip newline for comparison

      if (strcmp(trimmed, old_line) == 0) {
          fprintf(out, "%s\n", new_line);  // write the replacement
      } else {
          fputs(line, out);  // keep original line (with its own \n) unchanged
      }
  }

  fclose(in);
  fclose(out);

  if (rename(tmp_path, path) != 0) {
      perror("rename");
      return 1;
  }

  return 0;
}
