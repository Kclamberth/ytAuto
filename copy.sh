#!/bin/bash

# Set base directory to the location of copy.sh or default to "/media" if not found
baseDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
cd "$baseDir" || exit

youtubeDir="${baseDir}/youtube"
channelsFile="${youtubeDir}/channels.txt"

# Ensure youtube directory and channels.txt file exist
mkdir -p "$youtubeDir"
touch "$channelsFile"

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
done < "$channelsFile"

echo "Subdirectories for channels have been updated."

# Download content for each channel
counter=1
while IFS= read -r line; do
    channelName=$(echo "$line" | awk -F "@" '{print $2}')
    channelDir="${youtubeDir}/${channelName}"
    cd "$channelDir" || continue

    beforeDownload=$(ls -A | wc -l)
    yt-dlp "$line" --embed-chapters --embed-metadata --download-archive archive.txt
    afterDownload=$(ls -A | wc -l)

    if [ "$afterDownload" -gt "$beforeDownload" ]; then
        echo "${channelName} scan complete. New content archived on $(TZ='America/Los_Angeles' date '+%b-%d-%Y %H:%M:%S')." >> "${youtubeDir}/lastupdated.txt"
    else
        echo "${channelName} scan complete. No new content as of $(TZ='America/Los_Angeles' date '+%b-%d-%Y %H:%M:%S')." >> "${youtubeDir}/lastupdated.txt"
    fi

    ((counter++))
done < "$channelsFile"

echo "Download process complete. Check ${youtubeDir}/lastupdated.txt for updates."

# Optional: Notify completion via Discord or another method
$(which bash) "${baseDir}"/discordbot.sh
