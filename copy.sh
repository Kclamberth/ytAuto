#!/bin/bash

counter=1
e0=$(pwd)

for dir in $e0/shows/*
do
	cd $dir
	e1=$(ls -l | wc -l) #counts number of items in directory BEFORE yt-dlp runs

	#downloads everything on a youtube channel that is NOT already in the archive.txt 
	yt-dlp $(cat $e0/channels.txt | sed -n "$counter"p) --download-archive archive.txt

	e2=$(ls -l | wc -l) #counts number of items in directory AFTER yt-dlp runs

	#grabs the first video in the folder and stores it's name in a variable
	e3=$(ls -lt | grep webm | head -n 1 | awk -F " " '{ for (i=9; i<=NF; i++) print $i }' | paste -s -d " ")

	cd $e0

	if [ $e2 -gt $e1 ] #if items in directory grew after yt-dlp runs
	then
    	echo $e3 downloaded on $(date) in $dir  >> lastupdated.txt
	else
    	echo No new videos as of $(date) in $dir >> lastupdated.txt
	fi
	counter=$(expr $counter + 1)
done

e4=$(exp $(ls -l | wc -l) - 1) #number of items in dir (not counting total line)
cat lastupdated.txt | tail -$e4 #prints out what changed since the script last ran


