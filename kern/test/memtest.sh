gcc -O0 -fno-builtin -g -Wall -DMTSAFE=0 -DHACKS=1 -DSLABHUGELK=1 -DMAGLK=1 -DNEWLK=1 -DMAGBITMAP=1 -DMEMTEST=1 -I../.. -I../../usr/lib -o memtest memtest.c ../mem/mag.c ../mem/slab.c -pthread
