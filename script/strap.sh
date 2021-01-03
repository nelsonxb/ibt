#!/bin/bash

function x {
    echo "$@"
    "$@"
}

function findcc {
    if command -v gcc >/dev/null 2>&1; then
        echo gcc
    elif command -v clang >/dev/null 2>&1; then
        echo clang
    else
        echo "Can't find a \$CC" >&2
        exit 1
    fi
}

function findxx {
    case "$1" in
        *gcc) echo "${1%cc}++";;
        *clang) echo "$1++";;
        *) echo "$1";;
    esac
}

set -e

test -n "$CC" || CC="$(findcc)"
test -n "$CXX" || CXX="$(findxx "$CC")"
CFLAGS+="-Wall -Werror"
echo "CC=$CC; CXX=$CXX"

test -d build.strap || x mkdir -p build.strap
cd build.strap

objs=''
objmod=false
LD="$CC"

for cname in $(ls ../src); do
    cfile="../src/$cname"
    ofile="${cname}.o"

    case "$cname" in
        *.c++) LD="$CXX";;
    esac

    case "$cname" in
        *.c|*.c++)
            objs+=" $ofile"
            if [ ! -f "$ofile" -o "$cfile" -nt "$ofile" ]; then
                objmod=true
                case "$cname" in
                    *.c) x "$CC" $CFLAGS -c -o "$ofile" "$cfile";;
                    *.c++) x "$CXX" $CFLAGS -std=c++17 -c -o "$ofile" "$cfile";;
                esac
            fi
            ;;
    esac
done

if $objmod; then
    x "$LD" -o ibt $objs
fi
