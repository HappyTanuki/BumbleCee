sudo echo recording...
sudo perf record -F 99 -a -g -- sleep 60
sudo perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > stackflame.svg
sudo rm perf.data