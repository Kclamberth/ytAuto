#include "../include/config.h"
#include <stdio.h>
#include <stdlib.h>

int notify(const char *list_path, const char *log_path) {
  // check log for NEW keyword
  char *check_file;
  if (asprintf(&check_file, "tail -$(cat %s | wc -l) %s | grep NEW", list_path,
               log_path) == -1) {
    perror("notify asprintf");
    return -1;
  }
  FILE *file = popen(check_file, "r");

  int has_new = 0;
  char buffer[4096];
  if (file) {
    while (fgets(buffer, sizeof(buffer), file)) {
      has_new = 1;
      break;
    }
    pclose(file);
  }

  // NEW = Notify of updates, else no updates
  char *curl_cmd;
  if (has_new) {
    if (asprintf(&curl_cmd,
                 "curl -H \"Title: Media Server Updated\" "
                 "-H \"Tags: white_check_mark\" "
                 "-d \"$(cat %s | "
                 "tail -$(cat %s | wc -l) | "
                 "grep NEW | awk -F ' ' '{ $1=\"[\"; print }' | "
                 "awk '{ sub(\" \", \"\"); print }' | "
                 "sed 's/\"/\\\"/g')\" "
                 "%s",
                 log_path, list_path, NTFY_URL) == -1) {
      perror("notify asprintf");
      return -1;
    }

    if (system(curl_cmd) == -1) {
      perror("notify system");
      return -1;
    }
    free(check_file);
    free(curl_cmd);
    return 0;
  } else {
    if (asprintf(
            &curl_cmd,
            "curl -H \"Title: Media Server Updated\" "
            "-H \"Tags: x\" "
            "-d \"No new content found across $(cat %s | wc -l) channels.\" "
            "%s",
            list_path, NTFY_URL) == -1) {
      perror("notify asprintf");
      return -1;
    }

    if (system(curl_cmd) == -1) {
      perror("notify system");
      return -1;
    }
    free(curl_cmd);
    free(check_file);
    return 0;
  }

  printf("No notifications setup.");
  free(curl_cmd);
  free(check_file);
  return 1;
}
