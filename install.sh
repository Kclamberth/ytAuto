#!/bin/bash

BASE_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
YOUTUBE_DIR="${BASE_DIR}/youtube"

#colors
GREEN='\e[32m'
RED='\e[31m'
YELLOW='\e[33m'
RESET='\e[0m'

# dependencies check
dependencies=(yt-dlp jq curl bash)
missing_dependencies=0

echo "Checking for required dependencies..."
for dep in "${dependencies[@]}"; do
    if ! command -v "$dep" &> /dev/null; then
        echo -e "${RED}Missing dependency: $dep${RESET}"
        missing_dependencies=1
    fi
done

if [ $missing_dependencies -ne 0 ]; then
    echo -e "${RED}Please install missing dependencies before continuing.${RESET}"
    exit 1
fi

echo -e "${GREEN}All required dependencies are installed.${RESET}"


echo "Downloading scripts..."
curl -o "${BASE_DIR}/ytAuto.c" "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/refs/heads/main/ytAuto/ytAuto.c"
curl -o "${BASE_DIR}/discordbot.sh" "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/refs/heads/main/ytAuto/discordbot.sh"
echo -e "${GREEN}Scripts downloaded.${RESET}"

chmod +x "${BASE_DIR}/discordbot.sh"
gcc -o ytAuto ytAuto.c

# Final instructions
echo -e "${YELLOW}"
echo "Setup is complete! Here are the next steps:"
echo ""
echo "1. Edit ${YOUTUBE_DIR}/channels.txt and add YouTube channel links in the format:"
echo "   https://www.youtube.com/@exampleone"
echo ""
echo "2. Edit ${BASE_DIR}/discordbot.sh and insert your Discord webhook URL where indicated."
echo ""
echo "3. To automate the archiving process, consider setting up a cron job for ytAuto!"
echo ""
echo "Thank you for using kclamberth/yt-dlp-auto-updater!"
echo -e "${RESET}"

rm "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"/install.sh
rm "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"/ytAuto.c
