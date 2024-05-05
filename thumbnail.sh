#!/bin/bash

# Base URL for YouTube thumbnails
base_url="https://www.youtube.com/watch?v="

if [ -z "$1" ]; then
    echo "Error: No directory specified."
    exit 1
fi
directory="$1"

if [[ -f "${directory}/archive.txt" ]]; then
    echo "Processing directory: $directory"
    
    while IFS= read -r line; do
        # format "youtube <video_id>")
        video_id=$(echo "$line" | awk '{print $2}')
        video_file=$(find "$directory" -type f -name "*${video_id}*.*" ! -name "*.webp")

        if [[ -n "$video_file" ]]; then
            thumbnail_filename="${video_file%.*}.webp"

            if [ ! -f "$thumbnail_filename" ]; then
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
