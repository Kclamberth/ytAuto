#ifndef FS_H
#define FS_H

#include <sys/stat.h>
#include <sys/types.h>

int lock_file(const char *lock_path, const char *log_path);
int dir_exists(const char *directory, mode_t permissions);
int file_exists(const char *directory, const char *file, mode_t permissions,
                char *out);
int count_files(const char *directory);
int count_lines(const char *path);
int is_empty(const char *file);

#endif
