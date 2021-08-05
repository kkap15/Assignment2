#!/bin/zsh
tee client.in | ./client chat1.txt 2>&1 | tee client.out
