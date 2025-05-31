#include "../include/log.h"
#include "../include/config.h"
#include <stdio.h>
#include <sys/file.h>
#include <time.h>

void log_line(const char *log_path, const char *line) {
  FILE *log_file = fopen(log_path, "a");
  if (!log_file) {
    perror("log_line: open log");
    return;
  }

  // Lock log file
  int fd = fileno(log_file);
  if (flock(fd, LOCK_EX) != 0) {
    perror("log_line: flock");
    fclose(log_file);
    return;
  }

  // Setup time structure
  time_t timestamp = time(NULL);
  struct tm datetime = *localtime(&timestamp);
  char time_output[TIME_OUTPUT_SIZE];
  strftime(time_output, sizeof(time_output), "%Y-%m-%d %H:%M:%S", &datetime);

  // Write to log
  fprintf(log_file, "[%s] %s\n", time_output, line);
  fflush(log_file);

  // Unlock log file & close
  flock(fd, LOCK_UN);
  fclose(log_file);
}
