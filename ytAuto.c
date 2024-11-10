#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

int youtube_dir(char *working_dir) {
  // Check for youtube dir
  struct stat st = {0};
  if (stat(working_dir, &st) == -1) {
    // create youtube dir if not found
    int create_youtube_dir = mkdir("Youtube", 0777);
    if (create_youtube_dir != 0) {
      perror("Error making youtube directory\n");
      return -1;
    }
  } else {
    // youtube dir already exists
    printf("Youtube dir already exists\n");
  }
  return 0;
}

void ytdlp(char *full_link, char *channels_location) {
  printf("process path: %s\n", channels_location);
  chdir(channels_location);
  full_link[strlen(full_link) - 1] = '\0';
  char *arguments[] = {"yt-dlp",
                       full_link,
                       "--embed-chapters",
                       "--embed-metadata",
                       "--download-archive",
                       "archive.txt",
                       "--retries",
                       "20",
                       NULL};
  execvp(arguments[0], arguments);
  perror("execvp failed.");
  _exit(-1);
}

int count_files_in_dir(char *channel_location) {
  int file_count = 0;
  DIR *directory;
  struct dirent *entry;
  directory = opendir(channel_location);
  if (directory == NULL) {
    perror("Error counting files");
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

void log_line(FILE *log_file, const char *message) {
  // setup time structure
  time_t timestamp = time(NULL);
  struct tm datetime = *localtime(&timestamp);
  char time_output[50];
  strftime(time_output, sizeof(time_output), "%Y-%m-%d %H:%M:%S", &datetime);

  // lock log, write to it, unlock
  flockfile(log_file);
  fprintf(log_file, "[%s] %s\n", time_output, message);
  funlockfile(log_file);
  fflush(log_file);
}

void fork_process(char *full_link, char *channel_location, char *channel_name,
                  FILE *log_file) {
  int before_update = count_files_in_dir(channel_location);

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
    } else {
      // First child, waits for ytdlp to finish
      wait(NULL);
      int after_update = count_files_in_dir(channel_location);
      char log_message[1024];

      // dir grew, update log file
      if (after_update > before_update) {
        snprintf(log_message, sizeof(log_message), "%s: New content archived.",
                 channel_name);
      } else {
        snprintf(log_message, sizeof(log_message),
                 "%s: No new content archived.", channel_name);
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
  if (access(file_path, F_OK) == 0) {
    printf("%s already exists!\n", file_name);
  } else {
    // create file if not found
    FILE *fd = fopen(file_path, "w");
    if (fd == NULL) {
      perror("Error creating channels file.\n");
      return -1;
    } else {
      fclose(fd);
    }
  }
  return 0;
}

int channels_dir(char *working_dir, char *channels_path, char *log_path) {
  // Read each line from channels_path
  FILE *channels_list = fopen(channels_path, "r");
  FILE *log_file = fopen(log_path, "a");
  if (channels_list == NULL) {
    perror("Error opening channels file.\n");
    return -1;
  }

  char *buffer = NULL;
  size_t buffer_size;
  ssize_t channel_link;
  while ((channel_link = getline(&buffer, &buffer_size, channels_list)) != -1) {
    // grab full link for use in ytdlp below
    char *full_link = strdup(buffer);
    // find first occurence of '@'
    char *channel_name = strchr(buffer, '@');

    // replace '@' with '/'
    *channel_name = '/';
    // replace '\n' with '\0'
    channel_name[strlen(channel_name) - 1] = '\0';
    char channel_location[PATH_MAX];

    // create channel dir path
    snprintf(channel_location, PATH_MAX, "%s%s", working_dir, channel_name);
    channel_name++;

    // Check for existing dir
    struct stat st = {0};
    if (stat(channel_location, &st) == -1) {
      // create dir if not found
      int create_youtube_dir = mkdir(channel_location, 0777);
      if (create_youtube_dir != 0) {
        perror("Error making channel directory");
        return -1;
      }
    } else {
      // youtube dir already exists
      printf("%s dir already exists\n", channel_name);
    }

    fork_process(full_link, channel_location, channel_name, log_file);
    free(full_link);
  }

  int status;
  while (wait(&status) > 0)
    ;

  free(buffer);
  fclose(log_file);
  fclose(channels_list);
  return 0;
}

int main(int argc, char **argv) {
  // Setup working directory variables
  char working_dir[PATH_MAX];
  char channels_path[PATH_MAX];
  char log_path[PATH_MAX];
  char *youtube = "/Youtube";
  char *channels = "/channels.txt";
  char *log = "/lastupdated.txt";

  // Create working dir path
  if (getcwd(working_dir, sizeof(working_dir)) == NULL) {
    perror("Error getting working directory");
  } else {
    strcat(working_dir, youtube);
  }

  // Check/create youtube dir
  if (youtube_dir(working_dir) != 0) {
    return -1;
  }

  // Check/create channels file
  if (create_file(working_dir, channels_path, channels) != 0) {
    return -1;
  }

  // Check/create log file
  if (create_file(working_dir, log_path, log) != 0) {
    return -1;
  }

  // Check/create/update channel dirs
  if (channels_dir(working_dir, channels_path, log_path) != 0) {
    return -1;
  }

  printf("\nFinished updating channels.\n");

  return 0;
}
