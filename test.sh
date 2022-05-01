#!/bin/bash

cd ~/15618-Final-Project

make clean; make

./benchmark -t 0 -d 6 -n 1 > results/benchmark0_depth6_threads1.txt
./benchmark -t 1 -d 6 -n 1 > results/benchmark1_depth6_threads1.txt
./benchmark -t 2 -d 6 -n 1 > results/benchmark2_depth6_threads1.txt

./benchmark -t 0 -d 6 -n 4 > results/benchmark0_depth6_threads4.txt
./benchmark -t 1 -d 6 -n 4 > results/benchmark1_depth6_threads4.txt
./benchmark -t 2 -d 6 -n 4 > results/benchmark2_depth6_threads4.txt

./benchmark -t 0 -d 6 -n 16 > results/benchmark0_depth6_threads16.txt
./benchmark -t 1 -d 6 -n 16 > results/benchmark1_depth6_threads16.txt
./benchmark -t 2 -d 6 -n 16 > results/benchmark2_depth6_threads16.txt

./benchmark -t 0 -d 6 -n 64 > results/benchmark0_depth6_threads64.txt
./benchmark -t 1 -d 6 -n 64 > results/benchmark1_depth6_threads64.txt
./benchmark -t 2 -d 6 -n 64 > results/benchmark2_depth6_threads64.txt