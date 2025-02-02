#!/bin/bash
if [ "$(id -u)" -ne 0 ]; then
    exec sudo "$0" "$@"
fi
cd build
cmake .. && make
cd ..
cat password | docker login -u happytanuki12 --password-stdin
docker build --tag happytanuki12/bumblebee:latest .
docker push happytanuki12/bumblebee:latest