# yt-dlp-auto-updater
This is a powerful terminal based C program for use in cron jobs to automate the task of downloading youtube videos via yt-dlp. 

**NOTE:**
For use in jellyfin, I have the program in the parent directory of shows (/media/tvshows) and movies (/media/movies).
(EX: /media/Youtube is where I want my youtube channels, so I have the program located in /media)

**The first time you run this program, you need to run it from the directory you intend the youtube directory to be (EX. I ran it from /media the first time), this is because all the subdirectories will be created in that directory!**

## Arguments
```ytAuto``` : No arguments simply runs the program.

```ytAuto -l```: List the current tracked channels inside the channels.list file.

```ytAuto -a {argument}```: Add a channel to the channel list, channel argument must be in https://www.youtube.com/@{channel_name} format.

```ytAuto -r {argument}```: Remove a channel from the channel list. Argument must be exact match to item on channel list.

## How it Works

This C script manages YouTube channel content efficiently by creating and updating directory structures for each channel, leveraging parallel processing to run multiple yt-dlp download tasks simultaneously. It creates subdirectories for each channel, monitors file counts to detect new content, and logs updates in a `channels.log` file. To ensure reliability, the script checks for and creates necessary files and directories (`channels.list` and `youtube`) if missing. Note: For user notifications, the script can call an external Bash script to send updates to a Discord webhook, providing real-time information on channel activities.

- **Directory and File Setup**: The script automatically creates a `youtube` directory in a specified base location (e.g., `/media/youtube`). Inside this directory, it initializes a `channels.list` file.

- **Channel Links**: You need to populate the `channels.list` file with YouTube channel URLs in the format `https://www.youtube.com/@exampleone`. The use of the simplified format with `@` is crucial for the script to correctly parse and handle the channel names. See the argument section for how to do this.

- **Subdirectory Creation**: Upon subsequent executions of `ytAuto`, the script reads the channel links from `channels.list` and creates corresponding subdirectories within the `youtube` directory for each channel (e.g., `/media/youtube/exampleone` for `https://www.youtube.com/@exampleone`). This organization facilitates neat storage and easy management of downloaded content.

- **Content Downloading**: Using `yt-dlp`, the script downloads videos from the specified channels, placing them into their respective subdirectories. Each download updates the archive, ensuring only new content is downloaded in subsequent runs.

- **Managing Channels**: To add more channels, simply insert new links into `channels.list`. Removing a channel involves deleting its link from `channels.list` and manually removing the corresponding subdirectory (e.g., `/media/youtube/exampleone`). See the argument section for how to do this.

This setup allows for easy expansion or reduction of your archived content by merely editing the `channels.list` file. Automated runs through cron jobs can ensure your media server remains up-to-date without manual intervention.

### Optional Discord Notifications

By using the optional `discordbot.sh` script, you can receive updates via Discord notifications. This script parses the `channels.log` file to send a message about newly archived content or to notify when no new content has been detected. Configuration for Discord notifications involves setting up a webhook URL within the script.

**DEPENDENCIES**
-------------------------------------------------------------------------------------------------------
1.) yt-dlp

2.) ffmpeg

3.) jq

4.) curl
  
**INSTALLATION:**
-----------------------------------------------------------------------------------------------------------
To install and use, simply run the command below.

1.) ```curl https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/refs/heads/main/install.sh | bash```
