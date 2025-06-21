#include "../include/runner.h"
#include "../include/channel_utils.h"
#include "../include/config.h"
#include "../include/file_system.h"
#include "../include/process.h"
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void shuffle_channels(ChannelEntry *entries, int entryCount) {
  srand(time(NULL));
  for (int i = entryCount - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    ChannelEntry temp = entries[i];
    entries[i] = entries[j];
    entries[j] = temp;
  }
}

bool run_channels(const char *youtube_dir, const char *list_path,
                  const char *log_path,
                  const char *single_link) // NULL = update all
{
  char channel_dir[PATH_MAX];
  ChannelEntry entries[MAX_ENTRIES];
  int entryCount = 0;

  // Reads channels.csv, loads into entries
  if (load_channels(list_path, entries, &entryCount) != 0) {
    return false;
  }

  // Single link mode------------------------
  if (single_link) {
    for (int i = 0; i < entryCount; i++) {
      if (strcmp(entries[i].link, single_link) == 0) {
        snprintf(channel_dir, sizeof(channel_dir), "%s/%s", youtube_dir,
                 entries[i].dir_name);
        if (dir_exists(channel_dir, DIR_PERMS) != 0) {
          return false;
        }
        fork_process(entries[i].link, channel_dir, entries[i].dir_name,
                     log_path);
        while (wait(NULL) > 0)
          ;
        return true;
      }
    }
    printf("Link not found in list: %s\n", single_link);
    channel_list(list_path, "\nCurrent");
    return false;
  }

  // ALL links mode---------------------------
  long slots = sysconf(_SC_NPROCESSORS_ONLN);
  if (slots < 1) {
    slots = 1;
  }
  int running = 0;

  // Shuffle channels to ensure fair request distribution
  shuffle_channels(entries, entryCount);

  for (int i = 0; i < entryCount; i++) {
    if (validate_link(entries[i].link) != 0) {
      continue;
    }

    // Make channel dir (if needed)
    snprintf(channel_dir, sizeof(channel_dir), "%s/%s", youtube_dir,
             entries[i].dir_name);
    if (dir_exists(channel_dir, DIR_PERMS) != 0) {
      while (running-- > 0) {
        wait(NULL);
      }
      return false;
    }

    // Fork process
    fork_process(entries[i].link, channel_dir, entries[i].dir_name, log_path);
    running++;

    // Block processes >= core count
    int status;
    while (running >= slots) {
      pid_t pid = wait(&status);
      if (pid > 0) {
        --running;
      } else if (pid == -1 && errno == EINTR) {
        continue;
      }
    }
  }

  // wait for children to finish
  int status;
  while (wait(&status) > 0)
    ;
  return true;
}
