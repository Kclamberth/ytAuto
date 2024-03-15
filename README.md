# yt-dlp-auto-updater
This is a simple script for use in cron jobs to automate the task of downloading youtube videos via yt-dlp. 

**NOTE:**
To use this script you need to download install.sh and put it wherever you want.
For use in jellyfin, I have the script in the parent directory of shows.
(EX: /media/shows contains the shows I watch, so the script is located in /media)

**The first time you run this script, you need to run it from the directory you intend the youtube directory to be (EX. I ran it from /media the first time), this is because all the subdirectories will be created in that directory!**

**UPDATE 2024-03-15:**
--------------------------------------------------------------------------------------------------------------------------------
* Refactored code.

* Created install.sh script.

* Removed all hard path references. This code should now work on any Linux system, assuming dependencies are resolved.

* Modified copy.sh and discordbot.sh to accept timezones in the 3 date lines (2 in copy.sh, 1 in discordbot.sh).

* Added code blocks and reworded notifications for clarity in copy.sh and discordbot.sh.

## How it Works

When you run the `install.sh` script for the first time, it accomplishes several key tasks to set up your YouTube content archiving system:

- **Directory and File Setup**: The script automatically creates a `youtube` directory in a specified base location (e.g., `/media/youtube`). Inside this directory, it initializes a `channels.txt` file.

- **Channel Links**: You need to populate the `channels.txt` file with YouTube channel URLs in the format `https://www.youtube.com/@exampleone`. The use of the simplified format with `@` is crucial for the script to correctly parse and handle the channel names.

- **Subdirectory Creation**: Upon subsequent executions of copy.sh, the script reads the channel links from `channels.txt` and creates corresponding subdirectories within the `youtube` directory for each channel (e.g., `/media/youtube/exampleone` for `https://www.youtube.com/@exampleone`). This organization facilitates neat storage and easy management of downloaded content.

- **File Management**: The `channels.txt` file is automatically moved into the `youtube` directory if found outside. Additionally, a `lastupdated.txt` file is created within the same directory, acting as a log to track when videos were last pulled and archived from YouTube.

- **Content Downloading**: Using `yt-dlp`, the script downloads videos from the specified channels, placing them into their respective subdirectories. Each download updates the archive, ensuring only new content is downloaded in subsequent runs.

- **Managing Channels**: To add more channels, simply insert new links into `channels.txt`. Removing a channel involves deleting its link from `channels.txt` and manually removing the corresponding subdirectory (e.g., `/media/youtube/exampleone`).

This setup allows for easy expansion or reduction of your archived content by merely editing the `channels.txt` file. Automated runs through cron jobs can ensure your media server remains up-to-date without manual intervention.

### Optional Discord Notifications

By using the optional `discordbot.sh` script, you can receive updates via Discord notifications. This script parses the `lastupdated.txt` log to send a message about newly archived content or to notify when no new content has been detected. Configuration for Discord notifications involves setting up a webhook URL within the script.

**DEPENDENCIES**
-------------------------------------------------------------------------------------------------------
1.) yt-dlp

2.) ffmpeg

3.) jq

4.) curl
  
**INSTALLATION:**
-----------------------------------------------------------------------------------------------------------
1.) `wget "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/main/install.sh" && chmod +x install.sh`

2.) Simply run the install script via `./install.sh`.

3.) Follow the instructions of the install.sh or copy.sh if you run it immediately afterwards.

**NOTE:**
---------------------------------------------------------------------------------------------------------------------------------
If you wish to add more options to your yt-dlp download such as descriptions, metadata, etc then you need to modify this line!

**yt-dlp "$line" --embed-chapters --embed-metadata --download-archive archive.txt** in **line 38** of the script.


