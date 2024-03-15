#!/bin/bash

baseDir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
youtubeDir="${baseDir}/youtube"

#command to run
output="$(cat $youtubeDir/lastupdated.txt | tail -n"$(cat $youtubeDir/channels.txt | wc -l)" | grep "archived")"

if [[ -z "$output" ]]; then
	output="All channels scanned. No new content detected as of $(TZ='America/Los_Angeles' date '+%b-%d-%Y %H:%M:%S')."
fi

formattedOutput=$'```Update:\n'"${output}"$'\n```'

#Discord Webhook URL: find in server settings
webhook_url='[INSERT WEBHOOKS URL HERE]'

#Format as JSON
json_payload=$(jq -n --arg content "$formattedOutput" '{content: $content}')

#send to discord
curl -H "Content-Type: application/json" -X POST -d "$json_payload" $webhook_url
