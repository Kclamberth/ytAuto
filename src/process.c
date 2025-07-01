#include "../include/config.h"
#include "../include/file_system.h"
#include "../include/log.h"
#include "../include/ytdlp_download.h"
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void fork_process(const char *full_link, const char *channel_dir,
                  const char *channel_name, const char *log_path) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("process fork_process");
    return;
  }

  // Child
  if (pid == 0) {
    char archive_path[PATH_MAX];
    snprintf(archive_path, sizeof(archive_path), "%s/%s", channel_dir,
             ARCHIVE_FILE);
    int before = count_lines(archive_path);

    pid_t ytdlp = fork();
    if (ytdlp == -1) {
      perror("process fork_process[child]");
      _exit(1);
    }

    // Grandchild runs ytdlp
    if (ytdlp == 0) {
      spawn_ytdlp(full_link, channel_dir);
      _exit(0);
    }

    // Child waits for grandchild to finish checking count again
    time_t start = time(NULL);
    int status;

    while (1) {
      pid_t done = waitpid(ytdlp, &status, WNOHANG);
      if (done == ytdlp)
        break;

      if (difftime(time(NULL), start) > TIMEOUT_SEC) {
        kill(ytdlp, SIGTERM);
        sleep(5);
        kill(ytdlp, SIGKILL);

        char msg[LOG_MSG_SIZE];
        snprintf(msg, sizeof(msg), "[T/O] %s timed out after %d sec",
                 channel_name, TIMEOUT_SEC);
        log_line(log_path, msg);
        _exit(1);
      }
      sleep(1);
    }

    int after = count_lines(archive_path);

    // Log message
    char msg[LOG_MSG_SIZE];
    if (after > before) {
      snprintf(msg, sizeof msg, "[NEW] %s: %d video(s) archived.", channel_name,
               after - before);
    } else {
      snprintf(msg, sizeof msg, "[---] %s: No changes.", channel_name);
    }
    log_line(log_path, msg);
    _exit(0);
  }
}
