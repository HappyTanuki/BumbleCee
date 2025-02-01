FROM debian:sid
WORKDIR /
RUN apt update
RUN apt install -y curl
RUN apt install -y libopus0
RUN apt install -y tini
RUN apt install -y liboggz2
RUN apt install -y xz-utils
RUN apt install -y python3
RUN apt install -y python3-pip
RUN apt install -y ffmpeg
RUN apt install -y python3-certifi
RUN apt install -y python3-brotli
RUN apt install -y python3-websockets
RUN apt install -y python3-requests
RUN apt install -y python3-mutagen
RUN pip3 install --break-system-packages curl_cffi
RUN pip3 install --break-system-packages pycryptodome
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