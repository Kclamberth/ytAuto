#!/bin/bash

cd ~/Videos/ElizaASMR
yt-dlp $(cat ~/Pictures/channels.txt) | grep 'has already been downloaded' >> /dev/null 2>&1
e1=$?

if [ $e1 -ne 0 ]
then
    echo New Video downloaded on $(date) > lastupdated.txt
fi  


