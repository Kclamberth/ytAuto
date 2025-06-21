#include "../include/channel_utils.h"
#include "../include/file_system.h"
#include "../include/log.h"
#include "../include/notify.h"
#include "../include/runner.h"
#include <libgen.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

// parse args
int arg_parser(int argc, char **argv, char **paths) {
  char *youtube_dir = paths[0];
  char *list_path = paths[1];
  char *log_path = paths[2];

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      // Help
      if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
        printf("Usage: ytAuto [OPTIONS]\n\n"
               "Options:\n"
               "  -l, --list          List all tracked channels\n"
               "  -a, --append URL    Add a new channel (any yt-dlp supported "
               "link)\n"
               "  -r, --remove URL    Remove a channel (exact match required)\n"
               "  -s, --single URL    Download a single channel and add to "
               "tracking\n"
               "      --log           Display latest log entries\n"
               "  -h, --help          Show this help message\n");
        return 0;

        // List
      } else if ((strcmp(argv[i], "-l") == 0) ||
                 (strcmp(argv[i], "--list") == 0)) {
        if (is_empty(list_path) != 0) {
          return -1;
        }

        if (channel_list(list_path, "Current") != 0) {
          return -1;
        }

        // Log
      } else if (strcmp(argv[i], "--log") == 0) {
        if (is_empty(log_path) != 0) {
          return -1;
        }

        if (channel_log(log_path) != 0) {
          return -1;
        }

        // Append
      } else if ((strcmp(argv[i], "-a") == 0) ||
                 (strcmp(argv[i], "--append") == 0)) {
        if (i + 1 < argc) {
          if (channel_add(list_path, argv[i + 1]) != 0) {
            return -1;
          }
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-a' or '--append'.\n");
          return -1;
        }

        // Remove
      } else if ((strcmp(argv[i], "-r") == 0) ||
                 (strcmp(argv[i], "--remove") == 0)) {
        if (i + 1 < argc) {
          if (is_empty(list_path) != 0) {
            return -1;
          }

          if (channel_delete(list_path, argv[i + 1]) != 0) {
            return -1;
          }
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-r' or '--remove'.\n");
          return -1;
        }

        // Single
      } else if ((strcmp(argv[i], "-s") == 0) ||
                 (strcmp(argv[i], "--single") == 0)) {
        if (i + 1 < argc) {
          if (is_empty(list_path) != 0) {
            return -1;
          }

          log_line(log_path, "[RUN] ytAuto (single) started.");
          if (run_channels(youtube_dir, list_path, log_path, argv[i + 1]) ==
              false) {
            return -1;
          }

          log_line(log_path, "[END] ytAuto (single) finished.");
          notify(list_path, log_path);
          printf("Finished processing single channel.\n");
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-s' or '--single'.\n");
          return -1;
        }

        // Invalid argument
      } else {
        fprintf(stderr, "Error: unknown argument usage '%s'.\n", argv[i]);
        return -1;
      }
    }

    // No args (cronjobs)
  } else {
    // Check for channel file contents
    if (is_empty(list_path) != 0) {
      return -1;
    }

    log_line(log_path, "[RUN] ytAuto (multi) started.");
    // Use channel list to update channel dirs
    if (run_channels(youtube_dir, list_path, log_path, NULL) == false) {
      return -1;
    }

    log_line(log_path, "[END] ytAuto (multi) finished.");
    notify(list_path, log_path);
    printf("Finished processing all channels.\n");
  }

  return 0;
}
