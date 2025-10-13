#include "../include/ytdlp_download.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *args_template[] = {"yt-dlp",
                                      "--match-filters",
                                      "height>=?720",
                                      "--cookies",
                                      "../cookies.txt",
                                      "<LINK>",
                                      "--embed-chapters",
                                      "--embed-metadata",
                                      "--write-thumbnail",
                                      "--output",
                                      "%(title)s [%(id)s].%(ext)s",
                                      "--convert-thumbnails",
                                      "webp",
                                      "-o",
                                      "thumbnail:%(title)s [%(id)s].%(ext)s",
                                      "-o",
                                      "pl_thumbnail:",
                                      "--download-archive",
                                      "archive.txt",
                                      "--retries",
                                      "20",
                                      "--sleep-requests",
                                      "3",
                                      "--min-sleep-interval",
                                      "60",
                                      "--max-sleep-interval",
                                      "90",
                                      NULL};

int spawn_ytdlp(const char *link, const char *dest_dir) {
  if (chdir(dest_dir) != 0) {
    perror("ytdlp_download spawn_ytdlp");
    return -1;
  }

  // Count args
  size_t len = 0;
  while (args_template[len]) {
    ++len;
  }

  char **argv = calloc(len + 1, sizeof(char *));
  for (size_t i = 0; i < len; i++) {
    // iterate through args_template, if == <LINK> replace with actual link,
    // otherwise keep it same arg
    argv[i] =
        (char *)(strcmp(args_template[i], "<LINK>") == 0 ? link
                                                         : args_template[i]);
  }
  argv[len] = NULL;

  execvp(argv[0], argv);
  perror("execvp");
  free(argv);
  return -1;
}
