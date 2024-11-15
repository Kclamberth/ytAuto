#!/bin/bash

BASE_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
YOUTUBE_DIR="${BASE_DIR}/youtube"
UPDATE_DATE=$(TZ='America/Los_Angeles' date '+%b-%d-%Y')
WEBHOOK_URL='{WEBHOOK_URL}'

# Read updates into an array
IFS=$'\n' read -d '' -r -a updates < <(cat "$YOUTUBE_DIR"/.lastupdated.log | tail -n"$(grep -cv '^$' "$YOUTUBE_DIR"/.channels.list)" | grep "archived" && printf '\0')

if [ ${#updates[@]} -eq 0 ]; then
    # No updates found
    output="[$(TZ='America/Los_Angeles' date '+%H:%M:%S')] All channels scanned. No new content detected."
else
    # Process updates to format as requested
    for i in "${!updates[@]}"; do
        channelName=$(echo "${updates[$i]}" | sed -E 's/^\[.*\] (.*): New content archived\.$/\1/')
        timestamp=$(echo "${updates[$i]}" | grep -oE '[0-9]{2}:[0-9]{2}:[0-9]{2}')
        updates[i]="[${timestamp}] ${channelName}: New content archived."
    done

    # Join updates with newline character
    output=$(printf '%s\n' "${updates[@]}")
fi

# Format as JSON
json_payload=$(jq -n --arg title "**Update $UPDATE_DATE:**" --arg desc "$output" --argjson color 5814783 \
  '{embeds: [{title: $title, description: $desc, color: $color}]}')

# send to discord
curl -H "Content-Type: application/json" -X POST -d "$json_payload" $WEBHOOK_URL
