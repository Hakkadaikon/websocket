#!/bin/bash

# General gigabit tuning
# see: https://github.com/tranchiendang/linux-websocket-tuning/blob/7aae27c9a42b86b868a82b5de649f421f70480ea/README.md
###################################################
# sysctl -w net.core.somaxconn=16384
# sysctl -w net.core.rmem_max=16777216
# sysctl -w net.core.wmem_max=16777216
# sysctl -w net.ipv4.tcp_rmem="4096 87380 16777216"
# sysctl -w net.ipv4.tcp_wmem="4096 65536 16777216"
# sysctl -w net.ipv4.tcp_syncookies=1
# sysctl -w net.ipv4.tcp_mem="50576 64768 98152"
# sysctl -w net.core.netdev_max_backlog=2500
###################################################

# Default by ubuntu 20.04
###################################################
# net.core.somaxconn = 4096
# net.core.rmem_max = 212992
# net.core.wmem_max = 212992
# net.ipv4.tcp_rmem = 4096 131072 6291456
# net.ipv4.tcp_wmem = 4096 16384 4194304
# net.ipv4.tcp_syncookies = 1
# net.ipv4.tcp_mem = 9645 12862   19290
# net.core.netdev_max_backlog = 1000
###################################################

sysctl net.core.somaxconn
sysctl net.core.rmem_max
sysctl net.core.wmem_max
sysctl net.ipv4.tcp_rmem
sysctl net.ipv4.tcp_wmem
sysctl net.ipv4.tcp_syncookies
sysctl net.ipv4.tcp_mem
sysctl net.core.netdev_max_backlog
