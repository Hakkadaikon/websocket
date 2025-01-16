#!/bin/bash

cd $(dirname $0)

clang-tidy $(find ../src -name "*.[ch]")
