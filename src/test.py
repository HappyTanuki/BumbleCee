import yt_dlp
import json
import sys

if len(sys.argv) != 2:
    sys.exit()

ydl_opts = {
    'quiet': True,
    'clean_infojson': False,
    'default_search': 'ytsearch',
    'format': '251',
    'outtmpl': {'default': 'Temp/%(id)s.temp'},
    'overwrites': False,
    'writeinfojson': True }

with yt_dlp.YoutubeDL(ydl_opts) as ydl:
    info = ydl.extract_info(sys.argv[1], download=False)
    with open("out", "w") as f:
        f.write(json.dumps(ydl.sanitize_info(info)))