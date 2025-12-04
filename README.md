# Kclamberth/ytAuto

**ytAuto** is a powerful terminal-based C program designed to automate the task of archiving videos, primarily YouTube, using `yt-dlp`. Ideal for running via cron, this tool supports concurrent downloads, change detection, structured directory management, and optional push notifications.

## Features

- Downloads and archives content from multiple YouTube channels
- Concurrent processing for fast execution
- Smart change detection based on archive file
- Clean logging to `channels.log`
- Optional real-time notifications using **ntfy**
- Compatible with media servers like **Jellyfin**
- Simple CLI for managing tracked channels

## First-Time Setup
> **Important:** Run the program from the directory where you want your `youtube/` folder to be created.

**Example:**  
If you want your videos stored in `/media/youtube`, make sure you're inside `/media` the first time you run `ytAuto`:

```bash
cd /media
ytAuto
```
This will create a directory structure like:

```text
/media/
├── tvshows/
├── movies/
└── youtube/ <--------- ytAuto's working directory
    ├── examplechannel/
    ├── anotherchannel/
    ├── .channels.list
    └── .channels.log
```

## Arguments

```
ytAuto                # Updates all channels in parallel
ytAuto -h             # Show help message with list of arguments
ytAuto -l             # Lists all tracked channels
ytAuto -a URL         # Adds a new channel by URL (format: any yt-dlp supported links)
ytAuto -r URL         # Removes a channel by URL (exact match required)
ytAuto -s URL         # Downloads latest update for single link (Must match channel list link)
ytAuto --log          # Displays the latest log entries (one per tracked channel)
```

## How It Works

- **Channel Tracking**  
  Channels are tracked via a `channels.list` file inside the `youtube/` directory.

- **Directory Structure**  
  Each tracked channel has its own subdirectory (e.g., `youtube/exampleone/`), created automatically.

- **Parallel Downloads**  
  `ytAuto` uses `fork()` to download multiple channels in parallel, significantly reducing total run time.

- **Archive Management**  
  It uses `yt-dlp`'s archive feature to skip already-downloaded content and only fetch new videos.

- **Logging**  
  Each update is logged to `channels.log` with a timestamp, channel name, and whether new content was found.

- **Log Display**  
  Running with `--log` shows the latest log entry for each tracked channel.

- **Notifications (Optional)**  
  If configured, `ytAuto` can send push notifications about new content or no changes using `ntfy`.

## Optional ntfy Notifications

To receive push notifications, configure the `NTFY_URL` in `config.h`:

```c
#define NTFY_URL "https://ntfy.sh/mytopic"
```

## Dependencies
1.) yt-dlp **(REQUIRED)**

2.) ffmpeg **(REQUIRED)**
  
## Installation
To install, clone the repo and run the makefile.

1.) ```git clone https://github.com/Kclamberth/ytAuto && cd ./ytAuto```

2.) ```make```

3.) Add binary ```ytAuto``` to PATH.
