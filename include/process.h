#ifndef PROCESS_H
#define PROCESS_H
#include <stdio.h>

void fork_process(const char *full_link,
                  const char *channel_dir,
                  const char *channel_name,
                  FILE       *log_file);

#endif

