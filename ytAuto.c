#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
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
}

int channels_file(char *working_dir, char *channels_path, char *channels) {
  // create path to channels.txt
  snprintf(channels_path, PATH_MAX, "%s%s", working_dir, channels);

  // check if channels.txt exists
  if (access(channels_path, F_OK) == 0) {
    printf("Channels.txt already exists!\n");
  } else {
    // create channels.txt if not found
    FILE *channels_fd = fopen(channels_path, "w");
    if (channels_fd == NULL) {
      perror("Error creating channels file.\n");
      return -1;
    } else {
      fclose(channels_fd);
    }
  }
  return 0;
}

int channels_dir(char *working_dir, char *channels_path) {
  // Read each line from channels_path
  FILE *channels_list = fopen(channels_path, "r");
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

    pid_t pid = fork();
    // failed
    if (pid == -1) {
      perror("Fork failed");
      free(full_link);
      return -1;
      // child
    } else if (pid == 0) {
      ytdlp(full_link, channel_location);
      free(full_link);
      _exit(0);
    }
    free(full_link);
  }

  int status;
  while (wait(&status) > 0)
    ;

  free(buffer);
  fclose(channels_list);
  return 0;
}

int main(int argc, char **argv) {
  // Setup working directory variables
  char working_dir[PATH_MAX];
  char channels_path[PATH_MAX];
  char *youtube = "/Youtube";
  char *channels = "/channels.txt";

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
  if (channels_file(working_dir, channels_path, channels) != 0) {
    return -1;
  }

  // Check/create channel dirs
  if (channels_dir(working_dir, channels_path) != 0) {
    return -1;
  }

  return 0;
}
