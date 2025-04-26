#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

#define YOUTUBE "youtube"
#define OTHER "~Other"
#define CHANNELS_FILE "/.channels.list"
#define LOG_FILE "/.channels.log"
#define DISCORD "./ytDiscordBot.sh &"
#define LINK_STYLE "https://www.youtube.com/@"
#define NTFY_URL "<REPLACE_ME>"
#define PERMISSIONS 0755
#define FILE_PERMISSIONS 0664
#define TIME_OUTPUT_SIZE 50
#define LOG_MESSAGE_SIZE 1024
#define RETRIES "20"

int youtube_dir(char *working_dir) {
  struct stat st = {0};

  // youtube dir
  if (stat(working_dir, &st) == -1) {
    if (mkdir(working_dir, PERMISSIONS) != 0) {
      perror("Error making youtube directory");
      return -1;
    }
  }

  // youtube/other dir
  char other_dir[PATH_MAX] = {0};
  snprintf(other_dir, sizeof(other_dir), "%s/%s", working_dir, OTHER);

  if (stat(other_dir, &st) == -1) {
    if (mkdir(other_dir, PERMISSIONS) != 0) {
      perror("Error making youtube/other directory");
      return -1;
    }
  }

  return 0;
}

void ytdlp(char *full_link, char *channels_location) {
  if (chdir(channels_location) != 0) {
    perror("Error changing directories");
    _exit(-1);
  }
  if (full_link[strlen(full_link) - 1] == '\n') {
    full_link[strlen(full_link) - 1] = '\0';
  }

  // yt-dlp github contains list of args
  char *arguments[] = {"yt-dlp",
                       "--match-filters",
                       "height>=?720",
                       "--cookies",
                       "../cookies.txt",
                       full_link,
                       "--embed-chapters",
                       "--embed-metadata",
                       "--write-thumbnail",
                       "--output",
                       "%(title)s.%(ext)s",
                       "--convert-thumbnails",
                       "webp",
                       "-o",
                       "thumbnail:%(title)s.%(ext)s",
                       "-o",
                       "pl_thumbnail:",
                       "--download-archive",
                       "archive.txt",
                       "--retries",
                       RETRIES,
                       "--sleep-requests",
                       "1.25",
                       "--min-sleep-interval",
                       "60",
                       "--max-sleep-interval",
                       "90",
                       NULL};
  execvp(arguments[0], arguments);
  perror("execvp failed.\n");
  _exit(-1);
}

int count_files_in_dir(char *channel_location) {
  int file_count = 0;
  DIR *directory;
  struct dirent *entry;
  directory = opendir(channel_location);
  if (directory == NULL) {
    perror("Error counting files\n");
    return -1;
  }

  // iterate through each file in directory, until NULL is reached (EOD)
  while ((entry = readdir(directory)) != NULL) {
    // if entry is a reg file
    if (entry->d_type == DT_REG) {
      file_count++;
    }
  }

  closedir(directory);
  return file_count;
}

int channel_list(const char *channel_path, const char *argument) {
  FILE *channels_file = fopen(channel_path, "r");
  if (channels_file == NULL) {
    perror("Error reading to channel file\n");
    exit(-1);
  }

  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;
  printf("%s List of Channels:\n", argument);
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    printf("%s", buffer);
  }

  free(buffer);
  fclose(channels_file);
  exit(0);
}

void validate_link(const char *link) {
  // copy arg link into array size of LINK STYLE
  char sub_link[PATH_MAX];
  strncpy(sub_link, link, PATH_MAX - 1);

  // check link matches supported formats
  sub_link[strcspn(sub_link, "\n")] = 0;
  int is_youtube = strncmp(sub_link, LINK_STYLE, strlen(LINK_STYLE)) == 0;
  int is_model = strstr(sub_link, "/model/") != NULL;
  if (!is_youtube && !is_model) {
    fprintf(stderr, "Error: Invalid link format, check channels list.\n");
    exit(-1);
  }
}

int channel_add(const char *channel_path, const char *message) {
  FILE *channels_file = fopen(channel_path, "a+");
  if (channels_file == NULL) {
    perror("Error writing to channel file\n");
    exit(-1);
  }

  validate_link(message);

  // check if arg link already exists in channels file
  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    // skip empties
    if (line_length <= 1)
      continue;

    if (buffer[line_length - 1] == '\n') {
      buffer[line_length - 1] = '\0';
    }

    if (strcmp(message, buffer) == 0) {
      fprintf(stderr, "%s already exists in the channel file.\n", message);
      free(buffer);
      fclose(channels_file);
      exit(-1);
    }
  }

  // write to it channels if it passed validation
  if (fprintf(channels_file, "%s\n", message) < 0) {
    free(buffer);
    fclose(channels_file);
    perror("Error writing to channel file\n");
    exit(-1);
  }

  free(buffer);
  fclose(channels_file);
  channel_list(channel_path, "Updated");
  exit(0);
}

int channel_delete(const char *channel_path, const char *message) {
  FILE *channels_file = fopen(channel_path, "r");
  if (channels_file == NULL) {
    perror("Error writing to channel file\n");
    return -1;
  }

  // create tmp file to write to
  char tmp_channel_path[PATH_MAX];
  snprintf(tmp_channel_path, sizeof(tmp_channel_path), "%s.tmp", channel_path);

  FILE *temp_file = fopen(tmp_channel_path, "w");
  if (temp_file == NULL) {
    perror("Error writing to temp channel file\n");
    fclose(channels_file);
    return -1;
  }

  rewind(channels_file);

  // read contents of channel file, write to temp file
  char *buffer = NULL;
  size_t buffer_size;
  ssize_t line_length;
  while ((line_length = getline(&buffer, &buffer_size, channels_file)) != -1) {
    // skip empties
    if (line_length <= 1)
      continue;

    if (buffer[line_length - 1] == '\n') {
      buffer[line_length - 1] = '\0';
    }

    // if current line DOES NOT equal argument, add to temp file
    if (strcmp(buffer, message) != 0) {
      fprintf(temp_file, "%s\n", buffer);
    } else {
      continue;
    }
  }

  fclose(temp_file);
  fclose(channels_file);

  // delete original channel file by rename temp to channel file
  rename(tmp_channel_path, channel_path);

  free(buffer);
  channel_list(channel_path, "Updated");
  exit(0);
}

void log_line(FILE *log_file, const char *message) {
  // setup time structure
  time_t timestamp = time(NULL);
  struct tm datetime = *localtime(&timestamp);
  char time_output[TIME_OUTPUT_SIZE];
  strftime(time_output, sizeof(time_output), "%Y-%m-%d %H:%M:%S", &datetime);

  // lock log, write to it, unlock
  flockfile(log_file);
  fprintf(log_file, "[%s] %s\n", time_output, message);
  funlockfile(log_file);
  fflush(log_file);
}

void fork_process(char *full_link, char *channel_location, char *channel_name,
                  FILE *log_file) {
  pid_t pid = fork();
  // failed
  if (pid == -1) {
    perror("Fork failed");
    free(full_link);
    _exit(-1);
  } else if (pid == 0) {
    pid_t ytdlp_pid = fork();
    // failed
    if (ytdlp_pid == -1) {
      perror("Fork failed");
      free(full_link);
      _exit(-1);
    } else if (ytdlp_pid == 0) {
      // Second child (child of child)
      ytdlp(full_link, channel_location);
      _exit(0);
    } else {
      // First child - !!!POSSIBLE RACE CONDITION!!!
      int before_update = count_files_in_dir(channel_location);

      // wait for ytdlp process to finish
      waitpid(ytdlp_pid, NULL, 0);

      int after_update = count_files_in_dir(channel_location);
      char log_message[LOG_MESSAGE_SIZE];

      // dir grew, update log file
      if (after_update > before_update) {
        snprintf(log_message, sizeof(log_message),
                 "[NEW] %s: Content archived.", channel_name);
      } else {
        snprintf(log_message, sizeof(log_message), "[---] %s: No changes.",
                 channel_name);
      }
      log_line(log_file, log_message);

      free(full_link);
      _exit(0);
    }
  } else {
    // original parent
  }
}

int create_file(char *working_dir, char *file_path, char *file_name) {
  // create path to file
  snprintf(file_path, PATH_MAX, "%s%s", working_dir, file_name);

  // check if file exists
  if (access(file_path, F_OK) != 0) {
    // create file if not found
    int fd = open(file_path, O_CREAT | O_WRONLY, FILE_PERMISSIONS);
    if (fd == -1) {
      perror("Error creating channels file.\n");
      return -1;
    } else {
      close(fd);
    }
  }
  return 0;
}

int channels_dir(char *working_dir, char *channels_path, char *log_path,
                 char *single_link) {
  // Read each line from channels_path
  FILE *channels_list = fopen(channels_path, "r");
  if (channels_list == NULL) {
    perror("Error opening channels file.\n");
    return -1;
  }

  FILE *log_file = fopen(log_path, "a");
  if (log_file == NULL) {
    perror("Error opening log file.\n");
    fclose(channels_list);
    return -1;
  }

  char *buffer = NULL;
  size_t buffer_size = 0;
  ssize_t line_length;

  while (1) {
    // Single link mode
    if (single_link != NULL) {
      buffer = strdup(single_link);
      line_length = strlen(buffer);
    } else {
      // Channel list file mode
      line_length = getline(&buffer, &buffer_size, channels_list);
      if (line_length == -1)
        break;
    }
    // skip empties
    if (line_length <= 1) {
      continue;
    }

    // grab full link for use in ytdlp below
    char *full_link = strdup(buffer);

    // grab channel name (@channel and /model/channel supported formats)
    char *channel_name = NULL;
    char channel_location[PATH_MAX];
    if ((channel_name = strstr(buffer, "@")) != NULL) {
      channel_name += strlen("@");
      if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
      }

      // create channel dir path
      snprintf(channel_location, PATH_MAX, "%s/%s", working_dir, channel_name);

    } else if ((channel_name = strstr(buffer, "/model/")) != NULL) {
      channel_name += strlen("/model/");
      buffer[strlen(buffer) - 1] = '\0'; // remove newline

      // create channel dir path
      snprintf(channel_location, PATH_MAX, "%s/%s/%s", working_dir, OTHER,
               channel_name);
    }

    // Check for existing dir
    struct stat st = {0};
    if (stat(channel_location, &st) == -1) {
      // create dir if not found
      int create_youtube_dir = mkdir(channel_location, PERMISSIONS);
      if (create_youtube_dir != 0) {
        free(full_link);
        perror("Error making channel directory");
        return -1;
      }
    }

    fork_process(full_link, channel_location, channel_name, log_file);
    free(full_link);

    // Break out loop if only doing single
    if (single_link != NULL) {
      break;
    }
  }

  int status;
  while (wait(&status) > 0)
    ;

  free(buffer);
  fclose(log_file);
  fclose(channels_list);
  return 0;
}

int is_empty(char *channels_path) {
  struct stat st;
  if (stat(channels_path, &st) != 0) {
    perror("Error checking channels file.\n");
    return -1;
  } else {
    if (st.st_size == 0) {
      printf("Channels file is empty, please use './ytAuto -a "
             "https://www.youtube.com/@{channel_name}' "
             "to add a "
             "channel.\n");
      exit(1);
    }
  }
  return 0;
}

int notify(char *channels_path, char *log_path) {
  // check log for NEW keyword
  char *check_file;
  asprintf(&check_file, "tail -$(cat %s | wc -l) %s | grep NEW", channels_path,
           log_path);
  FILE *file = popen(check_file, "r");

  int has_new = 0;
  char buffer[4096];
  if (file) {
    while (fgets(buffer, sizeof(buffer), file)) {
      has_new = 1;
      break;
    }
    pclose(file);
  }

  // NEW = Notify of updates, else no updates
  char *curl_cmd;
  if (has_new) {
    asprintf(&curl_cmd,
             "curl -H \"Title: Media Server Updated\" "
             "-H \"Tags: white_check_mark\" "
             "-d \"$(cat %s | "
             "tail -$(cat %s | wc -l) | "
             "grep NEW | awk -F ' ' '{ $1=\"[\"; print }' | "
             "awk '{ sub(\" \", \"\"); print }')\" "
             "%s",
             log_path, channels_path,NTFY_URL);

    system(curl_cmd);
    free(check_file);
    free(curl_cmd);
    return 0;
  } else {
    asprintf(&curl_cmd,
             "curl -H \"Title: Media Server Updated\" "
             "-H \"Tags: x\" "
             "-d \"No new content found across $(cat %s | wc -l) channels.\" "
             "%s",
             channels_path,NTFY_URL);

    system(curl_cmd);
    free(curl_cmd);
    free(check_file);
    return 0;
  }

  printf("No notifications setup.");
  free(curl_cmd);
  free(check_file);
  return 1;
}

int main(int argc, char **argv) {
  // Setup working directory variables
  char temp_dir[PATH_MAX] = {0};
  char working_dir[PATH_MAX] = {0};
  char channels_path[PATH_MAX] = {0};
  char log_path[PATH_MAX] = {0};
  char discord_path[PATH_MAX] = {0};

  // Grab script's dir
  if (readlink("/proc/self/exe", temp_dir, sizeof(temp_dir) - 1) == -1) {
    perror("Error getting working directory");
    return -1;
  } else {
    // set up paths
    strcpy(working_dir, dirname(temp_dir));
    strcat(working_dir, "/");
    strcpy(discord_path, working_dir);
    strcat(discord_path, DISCORD);
    strcat(working_dir, YOUTUBE);
  }

  // Check for / create youtube dir
  if (youtube_dir(working_dir) != 0) {
    return -1;
  }

  // Check for / create channels file
  if (create_file(working_dir, channels_path, CHANNELS_FILE) != 0) {
    return -1;
  }

  // Check for / create log file
  if (create_file(working_dir, log_path, LOG_FILE) != 0) {
    return -1;
  }

  // parse args
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-l") == 0 && is_empty(channels_path) == 0) {
        channel_list(channels_path, "Current");
      } else if (strcmp(argv[i], "-a") == 0) {
        if (i + 1 < argc) {
          channel_add(channels_path, argv[i + 1]);
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-a'.\n");
          return -1;
        }
      } else if (strcmp(argv[i], "-r") == 0 && is_empty(channels_path) == 0) {
        if (i + 1 < argc) {
          channel_delete(channels_path, argv[i + 1]);
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-r.\n");
          return -1;
        }
      } else if ((strcmp(argv[i], "-s") == 0) ||
                 (strcmp(argv[i], "--single") == 0)) {
        if (i + 1 < argc) {
          validate_link(argv[i + 1]);
          channels_dir(working_dir, channels_path, log_path, argv[i + 1]);
          channel_add(channels_path, argv[i + 1]);
          i++;
        } else {
          fprintf(stderr, "Error: Missing argument for '-s' or '--single'.\n");
        }
      } else {
        fprintf(stderr, "Error: unknown argument usage '%s'.\n", argv[i]);
        return -1;
      }
    }
  } else {
    // Check for channel file contents
    if (is_empty(channels_path) != 0) {
      return -1;
    }
    // Use channel list to update channel dirs
    if (channels_dir(working_dir, channels_path, log_path, NULL) != 0) {
      return -1;
    }
  }

  notify(channels_path, log_path);
  printf("Finished updating channels.\n");

  return 0;
}
