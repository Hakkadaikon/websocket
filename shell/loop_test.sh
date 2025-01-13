#!/bin/bash

# Need iperf3.
# sudo apt install iperf3

iperf3 -s &
sleep 1
iperf3 -c localhost
pkill iperf3
