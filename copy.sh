#!/bin/bash

counter=1
e0=$(pwd) #store copy.sh directory

for dir in $e0/shows/*
do
    cd $dir #cd into specific channel folder
    e1=$(ls -l | wc -l) #counts number of items in directory BEFORE yt-dlp runs

    if [ -s "$e0/channels.txt" ]
    then
        #downloads everything on a youtube channel that is NOT already in the archive.txt
        yt-dlp $(cat $e0/channels.txt | sort | sed -n "$counter"p) --download-archive archive.txt

        e2=$(ls -l | wc -l) #counts number of items in directory AFTER yt-dlp runs

        #grabs the first video in the folder and stores it's name in a variable
        e3=$(ls -lt | grep --invert-match ".txt" | sed -n 2p | awk -F " " '{ for (i=9; i<=NF; i++) print $i }' | paste -s -d " ")

        cd $e0 #return to copy.sh directory

        if [ $e2 -gt $e1 ] #if items in directory grew after yt-dlp runs
	then
    	    echo $e3 downloaded on $(date) in $dir >> lastupdated.txt
	else
    	    echo "No new videos as of $(date) in $dir" >> lastupdated.txt
        fi
    else
        echo "channels.txt is empty or does not exist."
    fi

    counter=$(expr $counter + 1)

done

e4=$(cat $e0/channels.txt | wc -l) #number of items in dir (not counting total line)
echo " "
cat lastupdated.txt | tail -$e4 #prints out what changed for each item in channels.txt


