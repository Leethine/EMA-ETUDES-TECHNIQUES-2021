#!/bin/bash

# Note: it is required to change the LD_LIBRARY_PATH
# to your own opencv installation path
LD_LIBRARY_PATH=/home/lizian/.local/usr/opencv3.4.12/usr/local/lib64:/usr/lib64:$LD_LIBRARY_PATH

export LD_LIBRARY_PATH

EXE=$1

./$EXE
