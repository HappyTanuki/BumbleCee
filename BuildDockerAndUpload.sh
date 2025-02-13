#!/bin/bash
(
    cd build
    cmake .. && make
)
docker login -u happytanuki12 --password-stdin < password
docker build --tag happytanuki12/bumblebee:latest .
docker push happytanuki12/bumblebee:latest