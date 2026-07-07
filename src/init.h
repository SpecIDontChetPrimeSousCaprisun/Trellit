#ifndef INIT_H
#define INIT_H

static int remove_entry(const char *path, const struct stat *sb,
                         int typeflag, struct FTW *ftwbuf);

int remove_directory_recursive(const char *path);
int init();

#endif
