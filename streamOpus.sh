#!/bin/bash

YTDLP_CMD=$1
FFMPEG_CMD=$2
URL=$3

$YTDLP_CMD -o - --quiet --ignore-errors -f bestaudio $URL 2>/dev/null | \
$FFMPEG_CMD -hwaccel vaapi -i - -loglevel quiet -hide_banner -c:a copy -f opus - || \
$YTDLP_CMD -o - --quiet --ignore-errors -f bestaudio $URL 2>/dev/null | \
$FFMPEG_CMD -hwaccel vaapi -i - -loglevel quiet -hide_banner -f opus -c:a libopus -b:a 128k -ar 48000 -ac 2 -

#  -loglevel quiet