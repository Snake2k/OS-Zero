#! /bin/sh

gcc -g -Wall -O0 -I.. -I../../lib -o cell test.c x11.c cell.c ../../lib/zero/randmt32.c -lX11

