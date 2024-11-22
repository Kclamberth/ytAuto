#!/bin/bash

BASE_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
YOUTUBE_DIR="${BASE_DIR}/youtube"
UPDATE_DATE=$(TZ='America/Los_Angeles' date '+%b-%d-%Y')
WEBHOOK_URL='{WEBHOOK_URL}'

IFS=$'\n' read -d '' -r -a updates < <(cat "$YOUTUBE_DIR"/.channels.log | tail -n"$(grep -cv '^$' "$YOUTUBE_DIR"/.channels.list)" | grep "NEW" && printf '\0')

if [ ${#updates[@]} -eq 0 ]; then
    # no updates found
    output="[$(TZ='America/Los_Angeles' date '+%H:%M:%S')] All channels scanned. No changes."
else
    # process updates to format 
    for i in "${!updates[@]}"; do
        updates[i]=$(echo "${updates[i]}" | sed 's/\[[0-9]*-[0-9]*-[0-9]* \([0-9]*:[0-9]*:[0-9]*\)\]/[\1]/')
    done

    output=$(printf '%s\n' "${updates[@]}")
fi

# JSON
json_payload=$(jq -n --arg title "**Update $UPDATE_DATE:**" --arg desc "$output" --argjson color 5814783 \
  '{embeds: [{title: $title, description: $desc, color: $color}]}')

# send to discord
curl -H "Content-Type: application/json" -X POST -d "$json_payload" $WEBHOOK_URL
