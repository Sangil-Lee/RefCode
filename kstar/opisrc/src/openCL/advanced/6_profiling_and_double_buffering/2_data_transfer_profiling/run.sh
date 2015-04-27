#!/bin/bash
sizes=(512 1024 2048 4096 8192 16384 32768 65536 131072)
for size in ${sizes[*]}; do
  thorq --add --mode single --device gpu --base-dir here ./data_transfer_profiling $size
done
