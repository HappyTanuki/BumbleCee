FROM debian:sid
WORKDIR /
RUN apt-get update && \
    apt-get install -y curl libopus0 tini liboggz2 xz-utils ffmpeg python3 \
        python3-pip python3-certifi python3-brotli python3-websockets python3-requests python3-mutagen && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
RUN pip3 install --break-system-packages --no-cache-dir curl_cffi
RUN pip3 install --break-system-packages --no-cache-dir pycryptodome
RUN curl -Lo dpp.deb https://dl.dpp.dev/
RUN dpkg -i dpp.deb
RUN rm dpp.deb
RUN curl -LO https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp
RUN chmod +x ./yt-dlp
COPY ./build/BumbleCee /BumbleCee
COPY ./streamOpus.sh /streamOpus.sh
RUN chmod +x BumbleCee
RUN chmod +x streamOpus.sh
ENTRYPOINT ["/usr/bin/tini", "--", "./BumbleCee"]