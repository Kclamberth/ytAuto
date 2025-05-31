#ifndef CHANNEL_UTILS_H
#define CHANNEL_UTILS_H

typedef struct {
  char link[1024];
  char dir_name[256];
} ChannelEntry;

int channel_add(const char *list_path, const char *link);
int channel_delete(const char *list_path, const char *link);
int channel_log(const char *log_path);
int channel_list(const char *list_path, const char *title);
int load_channels(const char *list_path, ChannelEntry entries[],
                  int *entryCount);
int validate_link(const char *link);

#endif
