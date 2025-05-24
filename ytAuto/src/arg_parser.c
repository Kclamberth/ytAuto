#include "../include/channel_utils.h"
#include "../include/file_system.h"
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

      // List
      if (strcmp(argv[i], "-l") == 0) {
        if (is_empty(list_path) != 0) {
          return -1;
        }

        if (channel_list(list_path, "Current") != 0) {
          return -1;
        }

        // Append
      } else if (strcmp(argv[i], "-a") == 0) {
        if (i + 1 < argc) {
          if (channel_add(list_path, argv[i + 1]) != 0) {
            return -1;
          }
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-a'.\n");
          return -1;
        }

        // Remove
      } else if (strcmp(argv[i], "-r") == 0) {
        if (i + 1 < argc) {
          if (is_empty(list_path) != 0) {
            return -1;
          }

          if (channel_delete(list_path, argv[i + 1]) != 0) {
            return -1;
          }
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-r'.\n");
          return -1;
        }

        // Single
      } else if ((strcmp(argv[i], "-s") == 0) ||
                 (strcmp(argv[i], "--single") == 0)) {
        if (i + 1 < argc) {
          if (channel_add(list_path, argv[i + 1]) != 0) {
            return -1;
          }

          if (run_channels(youtube_dir, list_path, log_path, argv[i + 1]) ==
              false) {
            return -1;
          }
          notify(list_path, log_path);
          printf("Finished updating channels.\n");

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
    // Use channel list to update channel dirs
    if (run_channels(youtube_dir, list_path, log_path, NULL) == false) {
      return -1;
    }

    notify(list_path, log_path);
    printf("Finished updating channels.\n");
  }

  return 0;
}
