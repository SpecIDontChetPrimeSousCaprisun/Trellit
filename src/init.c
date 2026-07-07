#define _XOPEN_SOURCE 500   // nftw needs this defined before any includes
#include <sys/stat.h>
#include <ftw.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "init.h"

static int remove_entry(const char *path, const struct stat *sb,
                         int typeflag, struct FTW *ftwbuf) {
  (void)sb;      // unused, silence -Wextra warnings
  (void)ftwbuf;  // unused

  printf("Removing %s...\n", path);

  if (remove(path) != 0) {
      perror(path);
      return -1;   // tells nftw to stop walking
  }
  return 0;        // success, keep going
}

int remove_directory_recursive(const char *path) {
    return nftw(path, remove_entry, 16, FTW_DEPTH | FTW_PHYS);
}

static int checkForExistentFiles() {
  if (access(".trellit", F_OK) == 0) {
    bool shouldDelete = false;
    char buf[256];
    printf(".trellit is aldready a file / directory, do you want to remove it ? [Y/n]: ");
    fflush(stdout);  // see note below

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
      buf[strcspn(buf, "\n")] = '\0';
      if (strcasecmp(buf, "y") == 0 || strcasecmp(buf, "yes") == 0) shouldDelete = true; 
      else if (strcasecmp(buf, "n") != 0) printf("Unrecognised response : %s, defautling to no...\n", buf);
    } else {
      printf("No input recieved, cancelling...\n");
    }

    if (shouldDelete) {
      printf("Recursively deleting .trellit...\n");
      return remove_directory_recursive(".trellit");
    } else {
      fprintf(stderr, "Canceled by user\n");
      return 1;
    }
  }

  return 0;
}

int init() {
  if (checkForExistentFiles() == 1) return 1;

  mkdir(".trellit", 0755);
  printf("Sucessfully created the .trellit directory !\n");

  FILE *f = fopen(".gitignore", "w");
  if (f == NULL) {
      perror("fopen");
      return 1;
  }
  fprintf(f, "\ntrellit");
  fclose(f);
  printf("Sucessfully wrote .trellit to .gitignore\n");

  return 0;
}
