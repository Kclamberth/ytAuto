#!/bin/bash

BASE_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
YOUTUBE_DIR="${BASE_DIR}/youtube"

# colors
GREEN='\e[32m'
RED='\e[31m'
YELLOW='\e[33m'
RESET='\e[0m'

# dependencies check
dependencies=(yt-dlp jq curl bash)
missing_dependencies=0

echo ""
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
echo ""
echo -e "${YELLOW}Do you want to use the discord notification bot? (y/n)${RESET}"
read input
while [[ "$input" != "y" && "$input" != "n" ]]; do
  read -p "Enter (y/n): " input
done

echo "Downloading scripts..."
curl -o "${BASE_DIR}/ytAuto.c" "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/refs/heads/main/ytAuto/ytAuto.c"
if [ "$input" == 'y' ]; then
    curl -o "${BASE_DIR}/ytDiscordBot.sh" "https://raw.githubusercontent.com/Kclamberth/yt-dlp-auto-updater/refs/heads/main/ytAuto/ytDiscordBot.sh"
fi
echo -e "${GREEN}Scripts downloaded.${RESET}"

chmod +x "${BASE_DIR}/ytDiscordBot.sh"
gcc -o ytAuto ytAuto.c

# final instructions
echo -e "${YELLOW}"
echo "Setup is complete! Here are the next steps:"
echo ""
echo "1.) Edit '${BASE_DIR}/ytDiscordBot.sh' and insert your Discord webhook URL where indicated (If using discord)."
echo "2.) Use './ytAuto -a https://www.youtube.com/@{channel_name}' to track a channel."
echo "3.) Run the program via './ytAuto'"
echo "Thank you for using kclamberth/yt-dlp-auto-updater!"
echo -e "${RESET}"

# clean up
rm "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"/ytAuto.c
