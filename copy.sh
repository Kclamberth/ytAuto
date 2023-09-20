#!/bin/bash

counter=1
#e0=$(find / -name channels.txt 2>/dev/null | xargs dirname 2>/dev/null) #find channel.txt
#cd $e0

if [ -s channels.txt ]
then
	if [ ! $(find / -type d -iname youtube 2>/dev/null) ]   #if youtube directory DOES NOT exist, create youtube folder & subdirectories based off channels.txt
        then
	    mkdir youtube
	    e0=$(find / -name channels.txt 2>/dev/null | xargs dirname 2>/dev/null) #find channel.txt
	    cd $e0

	    echo "No youtube directory found or no channel folders found in $e0/youtube"
            echo "Creating youtube subdirectories based off of youtube links in the channels.txt file..."
            if [ -s $e0/channels.txt ]
	    then
	        e5=$(cat channels.txt | sort | wc -l)
	    else
		echo "channels.txt is empty, please add links to it to continue." | exit
	    fi

	    sleep 3

            for ((link = 1; link <= "$e5"; link ++))
            do
                e6=$(cat channels.txt | sort | awk -F "@" '{print $2}' | sed -n "$link"p)
                mkdir -p "$(pwd)/youtube/$e6"
                echo "$(pwd)/youtube/$e6 has been created"
            done

	echo " "
	sleep 5


	fi

	if [ -d youtube ] # if youtube directory does exist, continue
	then
	    e0=$(find / -name channels.txt 2>/dev/null | xargs dirname 2>/dev/null) #find channel.txt
	    e8=$(cat $e0/channels.txt | wc -l)
	    e9=$(ls -d $e0/youtube/* | grep -v .txt | wc -l)
	    if [ $e8 -gt $e9 ]
	    then
		echo "More channel links found than youtube subdirectories exist."
		echo "Creating youtube subdirectories based off of youtube links in the channels.txt file..."
		echo " "
		sleep 2
		e5=$(cat channels.txt | sort | wc -l)
		for ((link = 1; link <= "$e5"; link ++))
            	do
                    e6=$(cat channels.txt | sort | awk -F "@" '{print $2}' | sed -n "$link"p)
                    if [ -d $e0/youtube/$e6 ]
		    then
			sleep 1
		    else
		        mkdir -p "$(pwd)/youtube/$e6"
                        echo "$(pwd)/youtube/$e6 has been created"
		    fi
                done
	    echo " "
	    fi


	    #download channel contents into each subdirectory of youtube
    	    for dir in $e0/youtube/*
	    do
	        if [ -d $dir ]
		then
		    cd $dir #cd into specific channel folder
	            e1=$(ls -l | wc -l) #counts number of items in directory BEFORE yt-dlp runs

	            #downloads everything on a youtube channel that is NOT already in the archive.txt
	            yt-dlp $(cat $e0/channels.txt | sort | sed -n "$counter"p) --download-archive archive.txt

	            e2=$(ls -l | wc -l) #counts number of items in directory AFTER yt-dlp runs

	            #grabs the first video in the folder and stores it's name in a variable
	            e3=$(ls -lt | grep --invert-match ".txt" | sed -n 2p | awk -F " " '{ for (i=9; i<=NF; i++) print $i }' | paste -s -d " ")

	            cd $e0 #return to copy.sh directory

	            if [ $e2 -gt $e1 ] #if items in directory grew after yt-dlp runs
	            then
	    	        echo $e3 downloaded on $(date) in $dir >> $e0/youtube/lastupdated.txt
		    else
	    	        echo "No new videos as of $(date) in $dir" >> $e0/youtube/lastupdated.txt
	            fi
		counter=$(expr $counter + 1)

		fi

	    done

	    e4=$(cat $e0/channels.txt | wc -l) #number of items in dir (not counting total line)
	    echo " "
	    cat $e0/youtube/lastupdated.txt | tail -$e4 #prints out what changed for each item in channels.txt

	fi
else
	echo "Channels.txt is empty or does not exist."
	echo "Creating channels.txt..."
	echo " "
	sleep 2
	if [ -f channels.txt ]
	then
	    echo "channels.txt already exists but is empty."
	else
	    touch channels.txt
	fi
	e0=$(find / -name channels.txt 2>/dev/null | xargs dirname 2>/dev/null) #find channel.txt
	cd $e0
	echo "Ensure you populate channels.txt with links to youtube channels."
fi
