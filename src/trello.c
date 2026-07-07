#include "trello.h"
#include "config.h"
#include <curl/curl.h>

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userdata) {
  size_t chunk_size = size * nmemb;
  ResponseBuffer *resp = (ResponseBuffer *)userdata;

  /* Grow the buffer to fit the existing data + this new chunk + null terminator */
  char *new_data = realloc(resp->data, resp->size + chunk_size + 1);
  if (new_data == NULL) {
      fprintf(stderr, "realloc failed\n");
      return 0; /* returning != chunk_size tells curl an error occurred */
  }
  resp->data = new_data;

  memcpy(resp->data + resp->size, contents, chunk_size);
  resp->size += chunk_size;
  resp->data[resp->size] = '\0'; /* keep it null-terminated as a C string */

  return chunk_size; /* must return exactly this many bytes processed */
}

char *trello_get_workspaces() {
  AuthInfo *info = getAuthInfo();
  if (!info->success) {
    fprintf(stderr, "Failed to get auth info\n");
    return NULL;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "curl_easy_init failed\n");
    return NULL;
  }

  char url[512];
  snprintf(url, sizeof(url),
           "https://api.trello.com/1/members/me/organizations?key=%s&token=%s",
           info->key, info->token);

  ResponseBuffer resp = { .data = malloc(1), .size = 0 };
  resp.data[0] = '\0'; /* start as an empty string */

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "trellit/0.1");

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  long http_status = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
  if (http_status != 200) {
    fprintf(stderr, "trello API returned status %ld: %s\n", http_status, resp.data);
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  curl_easy_cleanup(curl);
  return resp.data;
}

char *trello_get_boards(char *workspaceId) {
  AuthInfo *info = getAuthInfo();
  if (!info->success) {
    fprintf(stderr, "Failed to get auth info\n");
    return NULL;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "curl_easy_init failed\n");
    return NULL;
  }

  char url[512];
  snprintf(url, sizeof(url),
           "https://api.trello.com/1/organizations/%s/boards?key=%s&token=%s&fields=id,name",
           workspaceId, info->key, info->token);

  ResponseBuffer resp = { .data = malloc(1), .size = 0 };
  resp.data[0] = '\0'; /* start as an empty string */

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "trellit/0.1");

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  long http_status = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
  if (http_status != 200) {
    fprintf(stderr, "trello API returned status %ld: %s\n", http_status, resp.data);
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  curl_easy_cleanup(curl);
  return resp.data;
}

char *trello_get_lists(char *id) {
  AuthInfo *info = getAuthInfo();
  if (!info->success) {
    fprintf(stderr, "Failed to get auth info\n");
    return NULL;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "curl_easy_init failed\n");
    return NULL;
  }

  char url[512];
  snprintf(url, sizeof(url),
           "https://api.trello.com/1/boards/%s/lists?key=%s&token=%s",
           id, info->key, info->token);

  ResponseBuffer resp = { .data = malloc(1), .size = 0 };
  resp.data[0] = '\0'; /* start as an empty string */

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "trellit/0.1");

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  long http_status = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
  if (http_status != 200) {
    fprintf(stderr, "trello API returned status %ld: %s\n", http_status, resp.data);
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  curl_easy_cleanup(curl);
  return resp.data;
}

char *trello_get_cards_from_list(char *id) {
  AuthInfo *info = getAuthInfo();
  if (!info->success) {
    fprintf(stderr, "Failed to get auth info\n");
    return NULL;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "curl_easy_init failed\n");
    return NULL;
  }

  char url[512];
  snprintf(url, sizeof(url),
           "https://api.trello.com/1/lists/%s/cards?key=%s&token=%s",
           id, info->key, info->token);

  ResponseBuffer resp = { .data = malloc(1), .size = 0 };
  resp.data[0] = '\0'; /* start as an empty string */

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "trellit/0.1");

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  long http_status = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
  if (http_status != 200) {
    fprintf(stderr, "trello API returned status %ld: %s\n", http_status, resp.data);
    free(resp.data);
    curl_easy_cleanup(curl);
    return NULL;
  }

  curl_easy_cleanup(curl);
  return resp.data;
}

