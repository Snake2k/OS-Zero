#! /bin/sh

CC=gcc
#CC=clang

#$CC -DPTHREAD=0 -D_REENTRANT -g -Wall -O -I. -I.. -I../../.. -I../../../usr/lib -fPIC -nostdinc -fno-builtin -shared -o zlibc.so *.c -pthread
$CC -DPTHREAD=1 -DMTSAFE=1 -D_REENTRANT -g -Wall -O -I. -I.. -I../../.. -I../../../usr/lib -fPIC -nostdinc -fno-builtin -shared -o zlibc.so *.c sys/zero/*.c -pthread

