#include "../include/config.h"
#include "../include/file_system.h"
#include "../include/log.h"
#include "../include/ytdlp_download.h"
#include <sys/wait.h>
#include <unistd.h>

void fork_process(const char *full_link, const char *channel_dir,
                  const char *channel_name, FILE *log_file) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("process fork_process");
    return;
  }

  // Child
  if (pid == 0) {
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

    // Child waits for grandchild to finish before checking numbers
    int before = count_files(channel_dir);
    waitpid(ytdlp, NULL, 0);
    int after = count_files(channel_dir);

    // Log message
    char msg[LOG_MSG_SIZE];
    snprintf(msg, sizeof msg, "%s %s: %s", after > before ? "[NEW]" : "[---]",
             channel_name,
             after > before ? "Content archived." : "No changes.");
    log_line(log_file, msg);
    _exit(0);
  }
}
