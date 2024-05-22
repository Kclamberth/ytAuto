#!/bin/bash

# Set base directory to the location of copy.sh or default to "/media" if not found
baseDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
cd "$baseDir" || exit

youtubeDir="${baseDir}/youtube"
channelsFile="${youtubeDir}/channels.txt"
LOG_FILE="${YOUTUBE_DIR}/copy_log.txt"

# Ensure youtube directory and channels.txt file exist
mkdir -p "$youtubeDir"
touch "$channelsFile"

# Check if channels.txt is empty or does not exist, exit with an error message if it is
if [ ! -s "$channelsFile" ]; then
    echo "[$(TZ='America/Los_Angeles' date '+%Y-%m-%d %H:%M:%S')] Error: '${youtubeDir}/channels.txt' is empty. Exiting script." | tee -a "$LOG_FILE"
    exit 1
fi

# Move channels.txt to youtube directory if it's still in the base directory
if [ -s "${baseDir}/channels.txt" ]; then
    mv "${baseDir}/channels.txt" "$channelsFile"
fi

# Read channel links from channels.txt and create subdirectories
# IFS= lets "read" read the entire line, whitespaces included.
# -r lets "read" read backslashes, useful for URLs
# stores it in variable line for processing
while IFS= read -r line; do
    channelName=$(echo "$line" | awk -F "@" '{print $2}')
    mkdir -p "${youtubeDir}/${channelName}"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] Created directory for ${channelName}" | tee -a "$LOG_FILE"
done < "$channelsFile"

echo "[$(TZ='America/Los_Angeles' date '+%Y-%m-%d %H:%M:%S')] Subdirectories for channels have been updated." | tee -a "$LOG_FILE"

# Download content for each channel
counter=1
while IFS= read -r line; do
    channelName=$(echo "$line" | awk -F "@" '{print $2}')
    channelDir="${youtubeDir}/${channelName}"
    cd "$channelDir" || continue

    echo "[$(TZ='America/Los_Angeles' date '+%Y-%m-%d %H:%M:%S')] Starting download for ${channelName}" | tee -a "$LOG_FILE"
    
    beforeDownload=$(ls -A | wc -l)
    yt-dlp "$line" --embed-chapters --embed-metadata --download-archive archive.txt
    afterDownload=$(ls -A | wc -l)

    if [ "$afterDownload" -gt "$beforeDownload" ]; then
        echo "[$(TZ='America/Los_Angeles' date '+%Y-%m-%d %H:%M:%S')] ${channelName} update complete. New content archived." | tee -a "$LOG_FILE"
        $(which bash) "${baseDir}/thumbnail.sh" "$channelDir"
    else
        echo "[$(TZ='America/Los_Angeles' date '+%Y-%m-%d %H:%M:%S')] ${channelName} update complete. No new content." | tee -a "$LOG_FILE"
    fi

    ((counter++))
done < "$channelsFile"

echo "[$(date '+%Y-%m-%d %H:%M:%S')] Download process complete. Check "$LOG_FILE" for updates." | tee -a "$LOG_FILE"

# Optional: Notify completion via Discord or another method
$(which bash) "${BASE_DIR}/discordbot.sh" | tee -a "$LOG_FILE" || exit
