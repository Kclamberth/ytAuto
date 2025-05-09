#include "../include/runner.h"
#include "../include/channel_utils.h"
#include "../include/config.h"
#include "../include/file_system.h"
#include "../include/process.h"
#include <linux/limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

bool run_channels(const char *youtube_dir, const char *list_path,
                  const char *log_path,
                  const char *single_link) // NULL = update all
{
  FILE *log_file = fopen(log_path, "a");
  if (!log_file) {
    perror("run_channels: open log");
    return false;
  }

  char channel_dir[PATH_MAX];
  ChannelEntry entries[MAX_ENTRIES];
  int entryCount = 0;

  // Reads channels.csv, loads into entries
  if (load_channels(list_path, entries, &entryCount) != 0) {
    fclose(log_file);
    return false;
  }

  // Single link mode------------------------
  if (single_link) {
    if (validate_link(single_link) == 0) {
      // channels -a (append to CSV first)
      snprintf(channel_dir, sizeof(channel_dir), "%s/%s", youtube_dir,
               entries[entryCount - 1].dir_name);
      if (dir_exists(channel_dir, DIR_PERMS) != 0) {
        return false;
      }
      fork_process(entries[entryCount - 1].link, channel_dir,
                   entries[entryCount - 1].dir_name, log_file);
    }
    fclose(log_file);

    // wait for child to finish
    while (wait(NULL) > 0)
      ;
    return true;
  }

  // ALL links mode---------------------------
  for (int i = 0; i < entryCount; i++) {
    if (validate_link(entries[i].link) != 0) {
      continue;
    }

    // (Make directory)  & fork
    snprintf(channel_dir, sizeof(channel_dir), "%s/%s", youtube_dir,
             entries[i].dir_name);
    if (dir_exists(channel_dir, DIR_PERMS) != 0) {
      return false;
    }
    fork_process(entries[i].link, channel_dir, entries[i].dir_name, log_file);
  }
  fclose(log_file);

  // wait for children to finish
  int status;
  while (wait(&status) > 0)
    ;
  return true;
}
