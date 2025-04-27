#include "../include/runner.h"
#include "../include/channel_utils.h"
#include "../include/config.h"
#include "../include/file_system.h"
#include "../include/process.h"
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static void process_channel(const char *full_link, const char *youtube_dir,
                            FILE *log_file) {
  // Get everything after '@'
  const char *at = strchr(full_link, '@');
  if (!at || *(at + 1) == '\0') {
    fputs("process_channel: bad link format, skipping\n", stderr);
    return;
  }
  const char *channel_name = at + 1;

  char channel_dir[PATH_MAX];
  snprintf(channel_dir, sizeof channel_dir, "%s/%s", youtube_dir, channel_name);

  // Create channel_dir if missing
  if (dir_exists(channel_dir, DIR_PERMS) != 0)
    return;

  fork_process(full_link, channel_dir, channel_name, log_file);
}

bool run_channels(const char *youtube_dir, const char *list_path,
                  const char *log_path,
                  const char *single_link) // NULL = update all
{
  FILE *log_file = fopen(log_path, "a");
  if (!log_file) {
    perror("run_channels: open log");
    return false;
  }

  // Single link mode
  if (single_link) {
    if (validate_link(single_link) == 0) {
      process_channel(single_link, youtube_dir, log_file);
    }
    fclose(log_file);

    // wait for children to finish
    int status;
    while (wait(&status) > 0)
      ;
    return true;
  }

  // List file mode
  FILE *list_file = fopen(list_path, "r");
  if (!list_file) {
    perror("run_channels: open channel list");
    fclose(log_file);
    return false;
  }

  char *buffer = NULL;
  size_t buffer_size = 0;
  ssize_t line_length;
  while ((line_length = getline(&buffer, &buffer_size, list_file)) != -1) {
    // skip empty lines
    if (line_length <= 1)
      continue;

    // strip newlines
    buffer[strcspn(buffer, "\n")] = '\0';

    // skip invalid links
    if (validate_link(buffer) == 0)
      process_channel(buffer, youtube_dir, log_file);
  }

  free(buffer);
  fclose(list_file);
  fclose(log_file);

  // wait for children to finish
  int status;
  while (wait(&status) > 0)
    ;
  return true;
}
