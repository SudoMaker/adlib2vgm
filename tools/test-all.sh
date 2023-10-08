#! /bin/bash

mkdir -p test

for i in ../examples/* ; do
    e=${i##*.}
    case "$e" in
        dat | 003 | bnk | snd | ins)
            ;;
        *)
            o=`basename "$i" "$e"`vgm
            ../build/adlib2vgm -i "$i" -o "test/$o"
            ;;
    esac
done

md5sum test/* > test.md5sum
diff test.md5sum reference.md5sum
