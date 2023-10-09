#! /bin/bash

if [ ! -e ../build/adlib2vgm ] ; then
    echo "unable to fine ../build/adlib2vgm" >&2
    exit 1
fi

mkdir -p test
> test.exit

for i in ../examples/* ; do
    e=${i##*.}
    case "$e" in
        dat | 003 | bnk | snd | ins)
            ;;
        *)
            o=`basename "$i" "$e"`vgm
            ../build/adlib2vgm -i "$i" -o "test/$o"
            echo "$? $i" >> test.exit
            ;;
    esac
done

diff -s reference.exit test.exit

md5sum test/* > test.md5sum
diff -s reference.md5sum test.md5sum

stat -c '%n %s' test/* > test.size
diff -s reference.size test.size
