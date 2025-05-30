#include "../include/arg_parser.h"
#include "../include/config.h"
#include "../include/file_system.h"
#include <libgen.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char **argv) {
  // Setup working directory variables
  char working_dir[PATH_MAX] = {0};
  char youtube_dir[PATH_MAX] = {0};
  char list_path[PATH_MAX] = {0};
  char log_path[PATH_MAX] = {0};
  char *paths[] = {youtube_dir, list_path, log_path};

  // Set working_dir to current dir
  if (getcwd(working_dir, sizeof(working_dir)) == NULL) {
    perror("Error getting working directory");
    return -1;
  } else {
    // set up paths
    snprintf(youtube_dir, sizeof(youtube_dir), "%s/%s", working_dir, YT_SUBDIR);
  }

  // Check for / create youtube dir
  if (dir_exists(youtube_dir, DIR_PERMS) != 0) {
    return -1;
  }

  // Check for / create channels file
  if (file_exists(youtube_dir, CHANNELS_FILE, FILE_PERMS, list_path) != 0) {
    return -1;
  }

  // Check for / create log file
  if (file_exists(youtube_dir, LOG_FILE, FILE_PERMS, log_path) != 0) {
    return -1;
  }

  // parse args
  if (arg_parser(argc, argv, paths) != 0) {
    return -1;
  }
  return 0;
}
