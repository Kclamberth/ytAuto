#include "../include/config.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int validate_link(const char *link) {
  // copy arg link into array size of LINK STYLE
  char sub_link[PATH_MAX];
  strncpy(sub_link, link, PATH_MAX - 1);

  // check link matches supported formats
  sub_link[strcspn(sub_link, "\n")] = 0;
  int is_youtube = strncmp(sub_link, LINK_STYLE, strlen(LINK_STYLE)) == 0;
  if (!is_youtube) {
    fprintf(stderr, "Error: Invalid link format, use %schannel\n", LINK_STYLE);
    return -1;
  }
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
  printf("%s List of Channels:\n", title);
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    printf("%s", buffer);
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

    if (strcmp(link, buffer) == 0) {
      fprintf(stderr, "%s already exists in the channel file.\n", link);
      free(buffer);
      fclose(channels_file);
      return -1;
    }
  }

  // write to it channels if it passed validation
  if (fprintf(channels_file, "%s\n", link) < 0) {
    free(buffer);
    fclose(channels_file);
    perror("Error writing to channel file\n");
    return -1;
  }

  free(buffer);
  fclose(channels_file);
  channel_list(list_path, "Updated");
  return 0;
}

int channel_delete(const char *list_path, const char *message) {
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

    // if current line DOES NOT equal argument, add to temp file
    if (strcmp(buffer, message) != 0) {
      fprintf(temp_file, "%s\n", buffer);
    } else {
      continue;
    }
  }

  fclose(temp_file);
  fclose(channels_file);

  // delete original channel file by rename temp to channel file
  rename(tmp_list_path, list_path);

  free(buffer);
  channel_list(list_path, "Updated");
  return 0;
}
