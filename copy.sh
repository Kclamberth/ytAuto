#!/bin/bash

cd ~/Videos/videodirectory
yt-dlp $(cat ~/Videos/channels.txt) --download-archive archive.txt 
e1=$?

if [ $e1 -eq 0 ]
then
    echo New Video downloaded on $(date) > lastupdated.txt
fi  


