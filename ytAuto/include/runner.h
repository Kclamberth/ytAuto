#ifndef RUNNER_H
#define RUNNER_H
#include <stdbool.h>

bool run_channels(const char *youtube_root,
                  const char *list_path,
                  const char *log_path,
                  const char *single_link);   // NULL = update all
#endif

