#include "../include/log.h"
#include "../include/config.h"
#include <stdio.h>
#include <time.h>

void log_line(FILE *log_file, const char *line) {
  // setup time structure
  time_t timestamp = time(NULL);
  struct tm datetime = *localtime(&timestamp);
  char time_output[TIME_OUTPUT_SIZE];
  strftime(time_output, sizeof(time_output), "%Y-%m-%d %H:%M:%S", &datetime);

  // lock log, write to it, unlock
  flockfile(log_file);
  fprintf(log_file, "[%s] %s\n", time_output, line);
  funlockfile(log_file);
  fflush(log_file);
}
