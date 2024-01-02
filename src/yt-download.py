import yt_dlp
import sys
import os
import json

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
    id = list()
    with open("out", "w") as f:
        f.write(json.dumps(ydl.sanitize_info(info)))
    with open("Music/Archive", "r") as f:
        ArchiveList = f.read().split("\n")
        if "entries" in info:
            if len(info["entries"]) != 0:
                for entry in info["entries"]:
                    if entry["id"] not in ArchiveList:
                        ydl.download(entry["webpage_url"])
                        os.system("echo " + entry["id"] + " >> Music/Archive")
                        os.system("yes n 2>/dev/null | ffmpeg -hide_banner -loglevel error -i \"" + "Temp/" + entry["id"] + ".temp" + "\" -c copy Music/" + entry["id"] + ".ogg")
                        os.system("mv Temp/" + entry["id"] + ".temp.info.json Music/" + entry["id"] + ".info.json")
                    id.append(entry["id"])
        else:
            if info["id"] not in ArchiveList:
                ydl.download(info["webpage_url"])
                os.system("echo " + info["id"] + " >> Music/Archive")
                os.system("yes n 2>/dev/null | ffmpeg -hide_banner -loglevel error -i \"" + "Temp/" + info["id"] + ".temp" + "\" -c copy Music/" + info["id"] + ".ogg")
                os.system("mv Temp/" + info["id"] + ".temp.info.json Music/" + info["id"] + ".info.json")
            id.append(info["id"])

    os.system("rm -f Temp/*.temp")
    os.system("rm -f Temp/*.json")

    with open("Temp/CurMusic", "w") as f:
        for item in id:
            f.write(item + "\n")