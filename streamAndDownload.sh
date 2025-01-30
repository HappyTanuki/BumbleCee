#!/bin/bash

YTDLP_CMD=$1
DOWNLOAD_ID=$2
FFMPEG_CMD=$3

$YTDLP_CMD -o - --quiet --ignore-errors -f bestaudio https://youtu.be/$DOWNLOAD_ID | \
$FFMPEG_CMD -i - -hide_banner -ar 48000 -ac 2 -c:a libopus -f ogg -