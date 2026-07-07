#define _XOPEN_SOURCE 500   // nftw needs this defined before any includes
#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <strings.h>
#include <stdbool.h>
#include "clone.h"
#include "list.h"
#include "init.h"
#include "config.h"
#include "trello.h"
#include "helper.h"

static int checkForExistentFiles(char *path) {
  if (access(path, F_OK) == 0) {
    bool shouldDelete = false;
    char buf[256];
    printf("%s is aldready a file / directory, do you want to remove it ? [Y/n]: ", path);
    fflush(stdout);

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
      buf[strcspn(buf, "\n")] = '\0';
      if (strcasecmp(buf, "y") == 0 || strcasecmp(buf, "yes") == 0) shouldDelete = true; 
      else if (strcasecmp(buf, "n") != 0) printf("Unrecognised response : %s, defautling to no...\n", buf);
    } else {
      printf("No input recieved, cancelling...\n");
    }

    if (shouldDelete) {
      printf("Recursively deleting %s...\n", path);
      return remove_directory_recursive(path);
    } else {
      fprintf(stderr, "Canceled by user\n");
      return 1;
    }
  }

  return 0;
}

typedef struct {
  char *workspaceId;
  char *boardId;
  bool success;
} BoardInfo;

static BoardInfo *getBoardInfo(char *argv[]) {
  BoardInfo *info = malloc(sizeof(BoardInfo));
  char *json = trello_get_workspaces();

  info->success = false;

  if (json == NULL) return info;

  info->success = true;
  info->workspaceId = "";
  info->boardId = "";

  cJSON *root = cJSON_Parse(json);

  int count = cJSON_GetArraySize(root);
  for (int i = 0; i < count; i++) {
    cJSON *workspace = cJSON_GetArrayItem(root, i);

    cJSON *id = cJSON_GetObjectItem(workspace, "id");
    cJSON *name = cJSON_GetObjectItem(workspace, "displayName");

    if (!(cJSON_IsString(id) && cJSON_IsString(name))) continue;
    if (strcmp(name->valuestring, argv[2]) != 0) continue;

    char *boardsJson = trello_get_boards(id->valuestring);

    if (boardsJson == NULL) {
      fprintf(stderr, "Failed to retrieve boards data !");
      continue;
    }

    cJSON *boardsRoot = cJSON_Parse(boardsJson);
    int boardsCount = cJSON_GetArraySize(boardsRoot);

    for (int ii = 0; ii < boardsCount; ii++) {
      cJSON *board = cJSON_GetArrayItem(boardsRoot, ii);

      cJSON *newBoardId = cJSON_GetObjectItem(board, "id");
      cJSON *boardName = cJSON_GetObjectItem(board, "name");

      if (!(cJSON_IsString(newBoardId) && cJSON_IsString(newBoardId))) continue;
      if (strcmp(boardName->valuestring, argv[3]) != 0) continue;

      info->boardId = strdup(newBoardId->valuestring);
      info->workspaceId = strdup(id->valuestring);

      break;
    }

    cJSON_Delete(boardsRoot);
    free(boardsJson);
  }
  
  cJSON_Delete(root);
  free(json);

  if (strcmp(info->workspaceId, "") == 0 || strcmp(info->boardId, "") == 0) {
    fprintf(stderr, "Couldn't resolve workspace / board id !\n");
    fprintf(stderr, "Run trellit list to get a list of all workspaces and boards\n");
    fprintf(stderr, "If it still doesn't work try giving the id yourself with the -i flag\n");

    if (strcmp(info->workspaceId, "") != 0) free(info->workspaceId);
    if (strcmp(info->boardId, "") != 0) free(info->boardId);

    info->success = false;
  }

  return info;
}

void cloneCards(char *path, char *id) {
  char *cardsJson = trello_get_cards_from_list(id);
  
  cJSON *root = cJSON_Parse(cardsJson);
  int count = cJSON_GetArraySize(root);
  for (int i = 0; i < count; i++) {
    cJSON *card = cJSON_GetArrayItem(root, i);
    cJSON *name = cJSON_GetObjectItem(card, "name");
    
    if (!cJSON_IsString(name)) continue;

    size_t filePathSize = strlen(path) + strlen("/") + strlen(name->valuestring) + 1;
    char *filePath = malloc(filePathSize);

    snprintf(filePath, filePathSize, "%s/%s", path, name->valuestring);

    FILE *f = fopen(filePath, "w");
    if (f == NULL) {
      perror("fopen");
      return;
    }

    cJSON *item = NULL;
    cJSON_ArrayForEach(item, card) {
      if (cJSON_IsString(item)) fprintf(f, "%s=%s\n", item->string, item->valuestring);
      else if (cJSON_IsNumber(item)) fprintf(f, "%s=%f\n", item->string, item->valuedouble);
      else if (cJSON_IsBool(item)) fprintf(f, "%s=%s\n", item->string, cJSON_IsTrue(item) ? "true" : "false");
    }
    fprintf(f, "order=%d", i);
    fclose(f);
    free(filePath);
  }

  cJSON_Delete(root);
}

int clone(int argc, char *argv[]) {
  Flags flags = parseFlags(argc, argv);
  BoardInfo *info;

  if (flags.argc < 4 || flags.argc > 5) {
    fprintf(stderr, "The clone subcommand requires 2 arguments !\n");
    fprintf(stderr, "Run trellit help for more details\n");
    return 1;
  }

  if (!flags.id) {
    info = getBoardInfo(argv);

    if (!info->success) return 1;

    printf("Found workspace id : %s\nAnd board id : %s\n", info->workspaceId, info->boardId);
  } else {
    info = malloc(sizeof(BoardInfo));
    info->success = true;
    info->workspaceId = flags.argv[2];
    info->boardId = flags.argv[3];
  }

  char *path;

  if (flags.argc > 4) path = argv[4];
  else path = ".trellit/board";

  if (checkForExistentFiles(path) == 1) return 1;

  printf("Cloning into %s...", path);

  char *lists = trello_get_lists(info->boardId);

  if (lists == NULL) return 1; 

  mkdir(path, 0755);

  cJSON *root = cJSON_Parse(lists);

  int count = cJSON_GetArraySize(root);
  for (int i = 0; i < count; i++) {
    cJSON *list = cJSON_GetArrayItem(root, i);

    cJSON *name = cJSON_GetObjectItem(list, "name");
    cJSON *id = cJSON_GetObjectItem(list, "id");

    if (!(cJSON_IsString(id) && cJSON_IsString(name))) continue;

    size_t listPathSize = strlen(path) + strlen(name->valuestring) + strlen("/") + 1;
    char *listPath = malloc(listPathSize);
    char *listInfoPath = malloc(listPathSize + strlen("/info"));

    snprintf(listPath, listPathSize, "%s/%s", path, name->valuestring);
    snprintf(listInfoPath, listPathSize + strlen("/info"), "%s/%s/info", path, name->valuestring);

    mkdir(listPath, 0755);

    FILE *f = fopen(listInfoPath, "w");
    if (f == NULL) {
      perror("fopen");
      return 1;
    }

    fprintf(f, "id=%s\n", id->valuestring);
    fclose(f);
    
    cloneCards(listPath, id->valuestring);
    free(listPath);
    free(listInfoPath);
  }

  cJSON_Delete(root);

  free(info);

  return 0;
}
