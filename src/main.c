#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "help.h"
#include "init.h"
#include "config.h"
#include "list.h"
#include "clone.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    help();
    return 0;
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);

  int returnVal = 0;

  if (strcmp(argv[1], "init") == 0) returnVal = init();
  else if (strcmp(argv[1], "config") == 0) returnVal = config(argc, argv);
  else if (strcmp(argv[1], "list") == 0) returnVal = listsubcmd();
  else if (strcmp(argv[1], "clone") == 0) returnVal = clone(argc, argv);
  else help();

  curl_global_cleanup();

  return returnVal;
}
