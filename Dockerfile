FROM debian:sid
WORKDIR /
RUN apt-get update
RUN apt-get install -y curl
RUN apt-get install -y libopus0
RUN apt-get install -y tini
RUN apt-get install -y liboggz2
RUN apt-get install -y xz-utils
RUN apt-get install -y python3
RUN apt-get install -y python3-pip
RUN apt-get install -y ffmpeg
RUN apt-get install -y python3-certifi
RUN apt-get install -y python3-brotli
RUN apt-get install -y python3-websockets
RUN apt-get install -y python3-requests
RUN apt-get install -y python3-mutagen
RUN pip3 install --break-system-packages --no-cache-dir curl_cffi
RUN pip3 install --break-system-packages --no-cache-dir pycryptodome
RUN curl -Lo dpp.deb https://dl.dpp.dev/
RUN dpkg -i dpp.deb
RUN rm dpp.deb
# RUN curl -LO https://github.com/BtbN/FFmpeg-Builds/releases/latest/download/ffmpeg-master-latest-linux64-gpl.tar.xz
# RUN tar -xf ffmpeg-master-latest-linux64-gpl.tar.xz
# RUN rm ffmpeg-master-latest-linux64-gpl.tar.xz
# RUN mv ffmpeg-master-latest-linux64-gpl ffmpeg
RUN curl -LO https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp
RUN chmod +x ./yt-dlp
COPY ./build/BumbleCee /BumbleCee
COPY ./streamOpus.sh /streamOpus.sh
# RUN curl -LO https://github.com/HappyTanuki/BumbleCee/releases/latest/download/BumbleCee
# RUN curl -LO https://github.com/HappyTanuki/BumbleCee/releases/latest/download/streamOpus.sh
RUN chmod +x BumbleCee
RUN chmod +x streamOpus.sh
ENTRYPOINT ["/usr/bin/tini", "--", "./BumbleCee"]