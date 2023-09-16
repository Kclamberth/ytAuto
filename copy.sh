#!/bin/bash

cd /data/jellyfin/shows/"SHOWNAME"
e0=$(pwd)
e1=$(ls -l | wc -l) #counts number of items in directory BEFORE yt-dlp runs

#downloads everything on a youtube channel that is NOT already in the archive.txt 
yt-dlp $(cat $e0/channels.txt) --download-archive archive.txt > /dev/null 2>&1

e2=$(ls -l | wc -l) #counts number of items in directory AFTER yt-dlp runs

#grabs the first video in the folder and stores it's name in a variable
e3=$(ls -lt | grep webm | head -n 1 | awk -F " " '{ for (i=9; i<=NF; i++) print $i }' | paste -s -d " ")

if [ $e2 -ge $e1 ] #if items in directory grew after yt-dlp runs
then
    echo $e3 downloaded on $(date) > lastupdated.txt
fi  


