#include "list.h"
#include "config.h"
#include "trello.h"
#include <stdio.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

int listsubcmd() {
  char *json = trello_get_workspaces();

  if (json == NULL) return 1;

  cJSON *root = cJSON_Parse(json);

  int count = cJSON_GetArraySize(root);
  for (int i = 0; i < count; i++) {
    cJSON *workspace = cJSON_GetArrayItem(root, i);

    cJSON *id = cJSON_GetObjectItem(workspace, "id");
    cJSON *name = cJSON_GetObjectItem(workspace, "displayName");

    if (cJSON_IsString(id) && cJSON_IsString(name)) {
      printf("%s (%s)\n", name->valuestring, id->valuestring);
      char *boardsJson = trello_get_boards(id->valuestring);

      if (boardsJson == NULL) {
        fprintf(stderr, "   Failed to retrieve boards data !");
        continue;
      }

      cJSON *boardsRoot = cJSON_Parse(boardsJson);
      int boardsCount = cJSON_GetArraySize(boardsRoot);

      for (int ii = 0; ii < boardsCount; ii++) {
        cJSON *board = cJSON_GetArrayItem(boardsRoot, ii);

        cJSON *boardId = cJSON_GetObjectItem(board, "id");
        cJSON *boardName = cJSON_GetObjectItem(board, "name");

        if (cJSON_IsString(boardId) && cJSON_IsString(boardName)) {
          printf("    %s (%s)\n", boardName->valuestring, boardId->valuestring);
        }
      }

      cJSON_Delete(boardsRoot);
      free(boardsJson);
    }
  }

  cJSON_Delete(root);  // frees the parsed tree itself
  free(json);

  return 0;
}
