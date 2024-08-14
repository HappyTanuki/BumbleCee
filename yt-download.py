import yt_dlp
import sys
import os

if len(sys.argv) != 2:
    sys.exit()

ydl_opts = {
    'quiet': True,
    'format': '251',
    'outtmpl': {'default': 'Temp/' + sys.argv[1]},
    'writeinfojson': True
}

with yt_dlp.YoutubeDL(ydl_opts) as ydl:
    info = ydl.extract_info("https://www.youtube.com/watch?v=" + sys.argv[1])
    os.system("yes n 2>/dev/null | ffmpeg -hide_banner -loglevel error -i \"" + "Temp/" + sys.argv[1] + "\" -c copy Music/" + sys.argv[1] + ".ogg > /dev/null 2> /dev/null")
    os.system("mv Temp/" + sys.argv[1] + ".info.json Music/" + sys.argv[1] + ".info.json > /dev/null 2> /dev/null")
    os.system("rm -rf Temp/ > /dev/null 2> /dev/null")