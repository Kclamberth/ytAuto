# yt-dlp-auto-updater
This is a simple script for use in cron jobs and to automate the task of downloading youtube videos via yt-dlp. 

**NOTE:**
To use this script you need to download copy.sh and put it wherever you want.
For use in jellyfin, I have the script in the parent directory of shows.
(EX: /media/shows contains the shows I watch, so the script is located in /media)

**The first time you run this script, you need to run it from the directory you intend the youtube directory to be (EX. I ran it from /media the first time), this is because all the subdirectories will be created in that directory!**

**UPDATE 2024-03-09:**
--------------------------------------------------------------------------------------------------------------------------------
Refactored code.

**HOW IT WORKS:**
--------------------------------------------------------------------------------------------------------------------------------
The first time you run this script, it will create a channels.txt file for you in the same directory as itself (for me that is in /media).
You simply add youtube channel links to this file in the **"https://www.youtube.com/@exampleone"** format. The simplified format with @ is important.

Once you add channel links to the channel.txt file, the next time you run the script it will automatically create a youtube directory in it's directory (/media if the copy.sh is located there) and then it will create subdirectories based off the channel link names. 
(EX: https://www.youtube.com/@exampleone creates a subdirectory called /media/youtube/exampleone in the /media/youtube directory.)

**This script will move channels.txt into the youtube directory, and it will create a lastupdated.txt in the youtube directory as well.**
The channels.txt is used to pull data from when you add links to it. The lastupdated.txt file will serve as a log of when your script last pulled videos from youtube. Yt-dlp will then download all the videos that the link contains, and add them to the their respective subdirectories.

If you add more links to the channels.txt at a later date, more subdirectories will automatically be created for you based off of the channel names,
so you simply need to run the script again or just let it run again via a cronjob (my preferred method).

**To delete any channels:** simply remove the link from the channels.txt file, and delete the directory of the channel in "/data/jellyfin/youtube".

**INSTALLATION:**
-----------------------------------------------------------------------------------------------------------
1.) wget "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/main/copy.sh"

2.) sudo chmod +x /path/to/copy.sh

3.) (OPTIONAL) Download discordbot.sh, and add your server's webhook url for use in notifications.

4.) Simply run the script via ./copy.sh

**NOTE:**
---------------------------------------------------------------------------------------------------------------------------------
If you wish to add more options to your yt-dlp download such as descriptions, metadata, etc then you need to modify this line!

**yt-dlp "$line" --embed-chapters --embed-metadata --download-archive archive.txt** in **line 38** of the script.


