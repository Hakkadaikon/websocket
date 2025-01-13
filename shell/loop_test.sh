#!/bin/bash

# Need iperf3.
# sudo apt install iperf3

iperf3 -s &
iperf3 -c localhost
