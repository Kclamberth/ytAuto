#!/bin/bash

# Installer script for YouTube Content Archiver and Discord Notifier

#colors
GREEN='\e[32m'
RED='\e[31m'
YELLOW='\e[33m'
RESET='\e[0m'

# Dependencies check
dependencies=(yt-dlp jq curl bash)
missing_dependencies=0

echo "Checking for required dependencies..."
for dep in "${dependencies[@]}"; do
    if ! command -v $dep &> /dev/null; then
        echo -e "${RED}Missing dependency: $dep${RESET}"
        missing_dependencies=1
    fi
done

if [ $missing_dependencies -ne 0 ]; then
    echo -e "${RED}Please install missing dependencies before continuing.${RESET}"
    exit 1
fi

echo -e "${GREEN}All required dependencies are installed.${RESET}"

# Set base directory and youtube directory
baseDir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
youtubeDir="${baseDir}/youtube"

# Create directories
echo "Creating directory structure..."
mkdir -p "$youtubeDir"
echo -e "${GREEN}Directory structure created at ${baseDir}.${RESET}"

# Download scripts
echo "Downloading scripts..."
curl -o "${baseDir}/copy.sh" "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/main/copy.sh"
curl -o "${baseDir}/discordbot.sh" "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/main/discordbot.sh"
echo -e "${GREEN}Scripts downloaded.${RESET}"

# Make scripts executable
chmod +x "${baseDir}/copy.sh"
chmod +x "${baseDir}/discordbot.sh"

# Setup channels.txt
if [ ! -f "${youtubeDir}/channels.txt" ]; then
    touch "${youtubeDir}/channels.txt"
    echo "Created channels.txt file. Please add YouTube channel links to it."
else
    echo "channels.txt already exists."
fi

# Final instructions
echo -e "${YELLOW}"
echo "Setup is complete! Here are the next steps:"
echo ""
echo "1. Edit ${youtubeDir}/channels.txt and add YouTube channel links in the format:"
echo "   https://www.youtube.com/@exampleone"
echo ""
echo "2. Edit ${baseDir}/discordbot.sh and insert your Discord webhook URL where indicated."
echo ""
echo "3. To automate the archiving process, consider setting up a cron job for copy.sh, like so:"
echo "   crontab -e"
echo "   And add:"
echo "   0 * * * * /path/to/your/copy.sh # Adjust schedule as needed"
echo ""
echo "Thank you for using kclamberth/yt-dlp-auto-updater!"
echo -e "${RESET}"

#rm $(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)/test.sh
