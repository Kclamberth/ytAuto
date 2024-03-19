#!/bin/bash

baseDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
youtubeDir="${baseDir}/youtube"

# Read updates into an array
IFS=$'\n' read -d '' -r -a updates < <(cat $youtubeDir/lastupdated.txt | tail -n"$(cat $youtubeDir/channels.txt | wc -l)" | grep "archived" && printf '\0')

if [ ${#updates[@]} -eq 0 ]; then
    # No updates found
    output="All channels scanned. No new content detected as of $(TZ='America/Los_Angeles' date '+%H:%M:%S')."
    updateDate=$(TZ='America/Los_Angeles' date '+%b-%d-%Y')
else
    # Extract the date from the last update for the title
    lastUpdate="${updates[${#updates[@]}-1]}"
    updateDate=$(echo "$lastUpdate" | grep -oE '[A-Za-z]{3}-[0-9]{2}-[0-9]{4}')

    # Process updates to format as requested
    for i in "${!updates[@]}"; do
        channelName=$(echo "${updates[$i]}" | sed -E 's/ update complete. New content archived on .*//')
        timestamp=$(echo "${updates[$i]}" | grep -oE '[0-9]{2}:[0-9]{2}:[0-9]{2}')
        updates[$i]="${channelName} update complete. New content archived at ${timestamp}."
    done

    # Join updates with newline character
    output=$(printf '%s\n' "${updates[@]}")
fi

webhook_url='https://discord.com/api/webhooks/1176388899782807582/NQ0VVWZDMBgxbZMaFXwcyE4jrSbEYgC78yAOApUZa3rOoprXZB7_qWFvtuqgoLJm-u5W'
# Format as JSON
json_payload=$(jq -n --arg title "**Update $updateDate:**" --arg desc "$output" --argjson color 5814783 \
  '{embeds: [{title: $title, description: $desc, color: $color}]}')
  
# send to discord
curl -H "Content-Type: application/json" -X POST -d "$json_payload" $webhook_url

