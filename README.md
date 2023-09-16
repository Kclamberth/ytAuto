# yt-dlp-auto-updater
Very simple script for use in cron jobs to automate the task of downloading youtube videos &amp; leaving a last updated text file.

NOTE:
To use this script you need to add a youtube channel link to the channels.txt file. 
You must also set the path to the show directory in the 3rd line of copy.sh.

Both copy.sh and channels.txt should be in the same directory as the youtube channel videos.Then, you can run this script manually, or you can set it up for use in a cronjob (that is what I prefer).

A lastupdated.txt file will be created in the directory. You can use it as a log file as to when your last download was.
