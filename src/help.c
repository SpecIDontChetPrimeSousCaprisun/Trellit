#include "help.h"
#include <stdio.h>
#include <string.h>

void help() {
  printf("usage : trellit <command> [arguments...]\n");
  printf("============================================\n");
  printf("============= Usefull commands =============\n");
  printf("============================================\n");
  printf("help : this command\n");
  printf("init : creates the .trellit directory\n");
  printf("version : prints out current version\n");
  printf("config [level, key, value] : sets the value to the key in the corresponding level config.\n");
  printf("                             Levels : local (this repo), global (your user)\n");
  printf("list : lists every workspaces and boards\n");
  printf("clone [workspace, board, path] : clones a board to the path, if no path is specified clones to .trellit\n");
  printf("clone -i [workspaceId, boardId, path]\n");
  printf("------------------------------------------\n");
  printf("------------- Board commands -------------\n");
  printf("------------------------------------------\n");
  printf("All subboard commands can be given the -i flag to be used with ids instead of names\n");
  printf("ls [list, path] : lists all the boards in a specified list from the cloned workspace, if the list is not specified then it lists all the lists of the cloned workspace\n");
  printf("check [list, card, path] : checks the specified card from the cloned workspace, will fail if the card is aldready checked unless the -f flag is used\n");
  printf("uncheck [list, card, path] : check but reversed\n");
  printf("togle [list, card, path] : togles the card's state\n");
  printf("archive [list, card, path] : archives a card / list if not cards given\n");
  printf("push [path] : pushes changes to trello\n");
  printf("pull [path] : pulls changes from trello\n");
}
