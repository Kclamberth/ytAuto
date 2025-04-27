#ifndef FS_H
#define FS_H

#include <sys/stat.h>
#include <sys/types.h>

int dir_exists(const char *directory, mode_t permissions);
int file_exists(const char *directory, const char *file, mode_t permissions, char *out);
int count_files(const char *directory);
int is_empty(const char *file);

#endif

