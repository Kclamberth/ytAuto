#!/bin/bash

#command to run
output="$(cat /media/youtube/lastupdated.txt | tail -n"$(cat /media/youtube/channels.txt | wc -l)" | grep "downloaded")"

if [[ -z "$output" ]]; then
	output="All channels scanned. No new content detected as of $(date '+%b-%d-%Y %H:%M:%S')."
fi

formattedOutput=$'```Update:\n'"${output}"$'\n```'

#Discord Webhook URL: find in server settings
webhook_url='[INSERT WEBHOOKS URL HERE]'

#Format as JSON
json_payload=$(jq -n --arg content "$formattedOutput" '{content: $content}')

#send to discord
curl -H "Content-Type: application/json" -X POST -d "$json_payload" $webhook_url
