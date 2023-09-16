# yt-dlp-auto-updater
Very simple script for use in cron jobs to automate the task of downloading youtube videos &amp; leaving a last updated text file.

NOTE:
To use this script you need to add a youtube channel link to the channels.txt file. 
You must also set the path to the show directory in the 3rd line of copy.sh.

All 3 items (channels.txt , copy.sh , and lastupdated.txt ) should be in the same directory as the youtube channel videos.
Then, you can run this script manually, or you can set it up for use in a cronjob (that is what I prefer).
