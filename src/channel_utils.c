#include "../include/channel_utils.h"
#include "../include/config.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int load_channels(const char *list_path, ChannelEntry entries[],
                  int *entryCount) {
  // LOADS channels from channels.csv into program
  FILE *channels_file = fopen(list_path, "r");
  if (channels_file == NULL) {
    perror("Error reading channels.csv");
    return -1;
  }

  char line[MAX_LINE];
  int count = 0;

  // Read through all channels in channels_file
  while (fgets(line, sizeof(line), channels_file) != NULL) {
    char *comma = strchr(line, ',');
    if (!comma) {
      continue;
    }

    // Split each line into 2 parts divided by comma
    *comma = '\0';
    char *link = line;
    char *dir_name = comma + 1;

    // remove newline from dir_name
    dir_name[strcspn(dir_name, "\r\n")] = '\0';

    // load into entries
    strncpy(entries[count].link, link, sizeof(entries[count].link));
    strncpy(entries[count].dir_name, dir_name, sizeof(entries[count].dir_name));
    count++;
  }

  fclose(channels_file);
  *entryCount = count;
  return 0;
}

int validate_link(const char *link) {
  // copy arg link into array size of LINK STYLE
  char sub_link[PATH_MAX];
  strncpy(sub_link, link, PATH_MAX - 1);

  // check link matches supported formats
  sub_link[strcspn(sub_link, "\n")] = 0;
  int is_youtube = strncmp(sub_link, LINK_STYLE, strlen(LINK_STYLE)) == 0;
  if (!is_youtube) {
    fprintf(stderr, "Error: Invalid link format, use %sexample.com\n",
            LINK_STYLE);
    return -1;
  }
  return 0;
}

int channel_log(const char *log_path) {
  FILE *log_file = fopen(log_path, "r");
  if (log_file == NULL) {
    perror("Error reading log file\n");
    return -1;
  }

  // Temp array to hold most recent RUN block
  char **lines = NULL;
  int line_count = 0;
  int default_cap = 64;
  lines = malloc(default_cap * sizeof(char *));
  if (!lines) {
    fclose(log_file);
    return -1;
  }

  // Allocate space for lines
  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;

  int in_block = 0;
  // Loop through log file lines
  while ((line_length = getline(&buffer, &buffer_size, log_file)) != -1) {
    // Store only most recent RUN
    if (strstr(buffer, "[RUN]")) {
      for (int i = 0; i < line_count; i++)
        free(lines[i]);
      line_count = 0;
      in_block = 1;
    }

    // If in a [RUN] block, store the line
    if (in_block) {
      // Too many lines, need realloc more mem
      if (line_count >= default_cap) {
        default_cap *= 2;
        char **new_lines = realloc(lines, default_cap * sizeof(char *));
        if (!new_lines) {
          perror("Realloc failed");
          fclose(log_file);
          free(buffer);
          for (int i = 0; i < line_count; i++)
            free(lines[i]);
          free(lines);
          return -1;
        }
        lines = new_lines;
      }
      // Puts line in buffer into the lines array
      lines[line_count++] = strdup(buffer);

      // Close block if [END] found
      if (strstr(buffer, "[END]")) {
        in_block = 0;
      }
    }
  }

  // Print out most recent log run
  printf("Last logs:\n");
  for (int i = 0; i < line_count; i++) {
    printf("%s", lines[i]);
    free(lines[i]);
  }

  free(lines);
  free(buffer);
  fclose(log_file);
  return 0;
}

int channel_list(const char *list_path, const char *title) {
  FILE *channels_file = fopen(list_path, "r");
  if (channels_file == NULL) {
    perror("Error reading to channel file\n");
    return -1;
  }

  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;
  int counter = 1;
  printf("%s List of Channels:\n", title);
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    printf("%d) %s", counter, buffer);
    counter++;
  }

  free(buffer);
  fclose(channels_file);
  return 0;
}

int channel_add(const char *list_path, const char *link) {
  FILE *channels_file = fopen(list_path, "a+");
  if (channels_file == NULL) {
    perror("Error writing to channel file\n");
    return -1;
  }

  if (validate_link(link) != 0) {
    fclose(channels_file);
    return -1;
  }

  // check if arg link already exists in channels file
  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    // skip empties
    if (line_length <= 1)
      continue;

    if (buffer[line_length - 1] == '\n') {
      buffer[line_length - 1] = '\0';
    }

    // Get the link part before the comma
    char *comma = strchr(buffer, ',');
    if (comma)
      *comma = '\0';

    if (strcmp(link, buffer) == 0) {
      fprintf(stderr, "%s already exists in the channel file.\n", link);
      free(buffer);
      fclose(channels_file);
      return -1;
    }
  }
  free(buffer);

  // Suggest a default dir name
  char defaultDir[256] = "channel";
  const char *lastSlash = strrchr(link, '/');
  const char *lastAt = strrchr(link, '@');
  const char *lastPart = (lastAt > lastSlash) ? lastAt : lastSlash;

  if (lastPart && *(lastPart + 1)) {
    strncpy(defaultDir, lastPart + 1, sizeof(defaultDir) - 1);
    char *c = strchr(defaultDir, '?');
    if (c) {
      *c = '\0';
    }
  }
  printf("Suggested directory name: %s\n", defaultDir);
  printf("Press Enter to accept, or type a new directory name: ");

  // Take user input if given, use that as dir_name
  char input[256];
  if (fgets(input, sizeof(input), stdin) == NULL) {
    fclose(channels_file);
    return -1;
  }
  input[strcspn(input, "\r\n")] = '\0';
  const char *dir_name = (*input) ? input : defaultDir;

  if (fprintf(channels_file, "%s,%s\n", link, dir_name) < 0) {
    perror("Error writing to channels file.");
    fclose(channels_file);
    return -1;
  }

  fflush(channels_file);
  fclose(channels_file);
  channel_list(list_path, "\nUpdated");
  return 0;
}

int channel_delete(const char *list_path, int channel_to_delete) {
  FILE *channels_file = fopen(list_path, "r");
  if (channels_file == NULL) {
    perror("Error writing to channel file\n");
    return -1;
  }

  // create tmp file to write to
  char tmp_list_path[PATH_MAX];
  snprintf(tmp_list_path, sizeof(tmp_list_path), "%s.tmp", list_path);

  FILE *temp_file = fopen(tmp_list_path, "w");
  if (temp_file == NULL) {
    perror("Error writing to temp channel file\n");
    fclose(channels_file);
    return -1;
  }

  rewind(channels_file);

  // read contents of channel file, write to temp file
  int counter = 1;
  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;
  int found = 0;
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    // skip empties
    if (line_length <= 1)
      continue;

    if (buffer[line_length - 1] == '\n') {
      buffer[line_length - 1] = '\0';
    }

    // if current line DOES NOT equal argument, add to temp file
    if (counter == channel_to_delete) {
      found = 1;
    } else {
      fprintf(temp_file, "%s\n", buffer);
    }
    counter++;
  }

  free(buffer);
  fclose(temp_file);
  fclose(channels_file);

  if (!found) {
    remove(tmp_list_path);
    fprintf(stderr, "No channel at index %d.\n", channel_to_delete);
    return -1;
  }

  // delete original channel file by rename temp to channel file
  rename(tmp_list_path, list_path);
  channel_list(list_path, "\nUpdated");
  return 0;
}
