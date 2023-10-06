#! /bin/bash

if [ $# != 1 ]; then
    echo "$0: usage: $0 filename.a2m" >&2
    exit 1
fi

if [ ! -e "$1" ]; then
    echo "$0: $1: file not found" >&2
    exit 1
fi

echo -n "version: "
od -N1 -j14 -An -tx1 "$1"
