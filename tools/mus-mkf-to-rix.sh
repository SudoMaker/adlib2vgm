#! /bin/bash

NUM=1

od -v -An -tx4 -w4 -j4 -N$((0x0160)) mus.mkf | {
    read END

    while [ "$END" != "00050e14" ] ; do
        START=$END
        read END

        OUTFILE=`printf "file%03d.rix" $NUM`
        NUM=$(($NUM+1))

        echo "$OUTFILE: $START - $END"

        SKIP=$((0x$START))
        COUNT=$((0x$END - 0x$START))

        dd if=mus.mkf of=$OUTFILE skip=$SKIP bs=1 count=$COUNT
    done
}

