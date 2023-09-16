# yt-dlp-auto-updater
Very simple script for use in cron jobs to automate the task of downloading youtube videos &amp; leaving a last updated text file.

This script will go into all of the jellyfin shows directories that you have created and automatically download a youtube channels' videos. 
It uses the channels.txt file to store channel links that you have added to it.

Duplicates are prevented via the archive.txt that is created in each shows directory.

**NOTE:
To use this script you need to:**

1.) Put copy.sh & channels.txt in your jellyfin directory.

2.) Add youtube channel links to the channels.txt file.

A lastupdated.txt file will be created in the jellyfin directory. 
You can use it as a log file to view when your last download was.
