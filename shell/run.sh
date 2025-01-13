#!/bin/bash

cd $(dirname $0)

cd .. && nix-build
result/bin/ws-server
