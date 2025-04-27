#ifndef CHANNEL_UTILS_H
#define CHANNEL_UTILS_H

int channel_add(const char *list_path, const char *link);
int channel_delete(const char *list_path, const char *link);
int channel_list(const char *list_path, const char *title);
int validate_link(const char *link);

#endif
