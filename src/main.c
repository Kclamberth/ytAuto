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
  // Setup working directory paths
  char working_dir[PATH_MAX] = {0};
  char youtube_dir[PATH_MAX] = {0};
  char list_path[PATH_MAX] = {0};
  char log_path[PATH_MAX] = {0};
  char lock_path[PATH_MAX] = {0};
  char *paths[] = {youtube_dir, list_path, log_path};

  // Set working_dir to current dir
  if (getcwd(working_dir, sizeof(working_dir)) == NULL) {
    perror("Error getting working directory");
    return -1;
  }

  // set up paths
  snprintf(youtube_dir, sizeof(youtube_dir), "%s/%s", working_dir, YT_SUBDIR);

  // Ensure all required paths and files exist
  if (dir_exists(youtube_dir, DIR_PERMS) != 0)
    return -1;
  if (file_exists(youtube_dir, CHANNELS_FILE, FILE_PERMS, list_path) != 0)
    return -1;
  if (file_exists(youtube_dir, LOG_FILE, FILE_PERMS, log_path) != 0)
    return -1;
  if (file_exists(TMP_DIR, LOCK_FILE, FILE_PERMS, lock_path) != 0)
    return -1;

  // Acquire lock
  int lock_fd = lock_file(lock_path, log_path);
  if (lock_fd == -1) {
    // Already running
    return 0;
  }

  // parse args
  if (arg_parser(argc, argv, paths) != 0) {
    return -1;
  }

  close(lock_fd);
  return 0;
}
