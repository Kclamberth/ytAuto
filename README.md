# yt-dlp-auto-updater
Very simple script for use in cron jobs to automate the task of downloading youtube videos &amp; leaving a last updated text file.

To use this script you need to add a youtube channel link to the channels.txt file. 

All 3 items (channels.txt , copy.sh , and lastupdated.txt ) should be in the same directory as the youtube channel videos.
Then, you can run this script manually, or you can set it up for use in a cronjob (that is what I prefer).

NOTE: You may have to change the 3rd line of copy.sh in order to set the path of your show folder for jellyfin.
