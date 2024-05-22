#!/bin/bash

# Base URL for YouTube thumbnails
base_url="https://www.youtube.com/watch?v="

# Check if a directory argument was passed
if [ -z "$1" ]; then
    echo "Error: No directory specified."
    exit 1
fi
directory="$1"

# Ensure there is an archive.txt file in the specified directory
if [[ -f "${directory}/archive.txt" ]]; then
    echo "Processing directory: $directory"

    # Read each line in the archive.txt
    while IFS= read -r line; do
        # Extract the YouTube video ID from the line (assuming format "youtube <video_id>")
        video_id=$(echo "$line" | awk '{print $2}')

        # Find a video file with this ID to get the full names
        video_file=$(find "$directory" -type f -name "*${video_id}*.*" ! -name "*.webp")

        if [[ -n "$video_file" ]]; then
            # Construct the thumbnail filename by replacing the video extension with .webp
            thumbnail_filename="${video_file%.*}.webp"

            # Check if the thumbnail already exists
            if [ ! -f "$thumbnail_filename" ]; then
                # Use yt-dlp to download the thumbnail for this video
                echo "Downloading thumbnail for video ID $video_id"
                yt-dlp --write-thumbnail --skip-download --convert-thumbnails webp --output "${thumbnail_filename%.webp}" "${base_url}${video_id}"
            else
                echo "Thumbnail for ${video_id} already exists."
            fi
        else
            echo "No matching video file found for ID $video_id in $directory"
        fi
    done < "${directory}/archive.txt"
else
    echo "No archive.txt found in $directory"
fi
