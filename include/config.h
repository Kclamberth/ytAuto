#ifndef CONFIG_H
#define CONFIG_H

// Filesystem
#define YT_SUBDIR "youtube"
#define TMP_DIR "/tmp"
#define CHANNELS_FILE ".channels.list"
#define LOG_FILE ".channels.log"
#define LOCK_FILE "ytAuto.lock"
#define ARCHIVE_FILE "archive.txt"

// Permissions
#define DIR_PERMS 0755
#define FILE_PERMS 0664

// Services
#define NTFY_URL "<REPLACE_ME>"

// Miscellaneous
#define LOG_MSG_SIZE 1024
#define MAX_LINE 1024
#define MAX_ENTRIES 256
#define TIME_OUTPUT_SIZE 50
#define TIMEOUT_SEC 3600
#define LINK_STYLE "https://"

#endif
