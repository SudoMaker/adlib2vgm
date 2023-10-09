#! /bin/bash

if [ ! -e ../build/adlib2vgm ] ; then
    echo "unable to fine ../build/adlib2vgm" >&2
    exit 1
fi

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
diff -s test.md5sum reference.md5sum

stat -c '%n %s' test/* > test.size
diff -s test.size reference.size
