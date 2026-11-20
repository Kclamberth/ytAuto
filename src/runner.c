#include "../include/runner.h"
#include "../include/channel_utils.h"
#include "../include/config.h"
#include "../include/file_system.h"
#include "../include/process.h"
#include <errno.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define GOLDEN_STEP UINT32_C(0x9E3779B9)

static int compare_channels(const void *a, const void *b) {
  const ChannelEntry *first = a;
  const ChannelEntry *second = b;

  return strcmp(first->link, second->link);
}

static int rotation_start(const char *youtube_dir, int count) {
  char path[PATH_MAX];
  char tmp[PATH_MAX];

  if (snprintf(path, sizeof(path), "%s/%s",
               youtube_dir, ROTATION_FILE) >= (int)sizeof(path) ||
      snprintf(tmp, sizeof(tmp), "%s/%s.XXXXXX",
               youtube_dir, ROTATION_FILE) >= (int)sizeof(tmp))
    return -1;

  uint32_t run = 0;

  // Read the persistent run counter.
  FILE *file = fopen(path, "r");
  if (file) {
    char buf[32] = {0};
    bool valid = fgets(buf, sizeof(buf), file) != NULL;
    size_t len = strspn(buf, "0123456789");

    valid = valid && len > 0 && len <= 10 &&
            (buf[len] == '\0' ||
             (buf[len] == '\n' && buf[len + 1] == '\0')) &&
            fgetc(file) == EOF;

    uintmax_t value = valid ? strtoumax(buf, NULL, 10) : 0;
    valid = valid && value <= UINT32_MAX && !ferror(file);

    if (fclose(file) != 0 || !valid)
      return -1;

    run = (uint32_t)value;
  } else if (errno != ENOENT) {
    return -1;
  }

  // Save the next run counter atomically.
  int fd = mkstemp(tmp);

  if (fd == -1)
    return -1;

  file = fdopen(fd, "w");

  if (!file) {
    close(fd);
    unlink(tmp);
    return -1;
  }

  bool ok = fprintf(file, "%" PRIu32 "\n",
                    (uint32_t)(run + 1u)) > 0;

  if (ok && fflush(file) != 0)
    ok = false;

  if (ok && fsync(fd) != 0)
    ok = false;

  if (fclose(file) != 0)
    ok = false;

  if (!ok || rename(tmp, path) != 0) {
    unlink(tmp);
    return -1;
  }

  // Calculate the golden-ratio starting position.
  uint32_t phase = (uint32_t)((uint64_t)run * GOLDEN_STEP);
  return (int)(((uint64_t)phase * (uint64_t)count) >> 32);
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

  if (entryCount == 0)
    return true;

  // Establish a deterministic channel order.
  qsort(entries, entryCount, sizeof(ChannelEntry),
        compare_channels);

  // Determine the starting position for this run.
  int start = rotation_start(youtube_dir, entryCount);

  if (start < 0) {
    fprintf(stderr, "Error updating channel rotation state.\n");
    return false;
  }

  printf("Channel rotation: starting at index %d of %d\n",
         start, entryCount);

  // Process channels in circular order.
  for (int offset = 0; offset < entryCount; offset++) {
    int i = (start + offset) % entryCount;
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
