#include "../include/file_system.h"
#include "../include/config.h"
#include <asm-generic/errno-base.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int dir_exists(const char *directory, mode_t permissions) {
  struct stat st = {0};

  if (stat(directory, &st) == -1) {
    if (mkdir(directory, permissions) != 0) {
      perror("File_system dir_exists");
      return -1;
    }
  }
  return 0;
}

int file_exists(const char *directory, const char *file, mode_t permissions,
                char *out) {
  snprintf(out, PATH_MAX, "%s/%s", directory, file);

  if (access(out, F_OK) != 0) {
    int fd = open(out, O_CREAT | O_WRONLY, permissions);
    if (fd == -1) {
      perror("File_system file_exists");
      return -1;
    }
    close(fd);
  }
  return 0;
}

int count_files(const char *directory) {
  int count = 0;
  DIR *dir = opendir(directory);
  if (!dir) {
    perror("File_system count_files");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir))) {
    if (entry->d_type == DT_REG) {
      ++count;
    }
  }

  closedir(dir);
  return count;
}

int count_lines(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    // File doesn't exist yet
    if (errno == ENOENT) {
      return 0;
    }

    // Other errors
    perror("File_system count_lines");
    return -1;
  }

  int count = 0;
  char line[MAX_LINE];
  while (fgets(line, sizeof(line), file)) {
    ++count;
  }

  fclose(file);
  return count;
}

int is_empty(const char *file) {
  struct stat st;
  if (stat(file, &st) != 0) {
    perror("Error checking channels file.\n");
    return -1;
  } else {
    if (st.st_size == 0) {
      printf("WARNING: Empty file '%s'\n", file);
      return 1;
    }
  }
  return 0;
}
