gcc -O -g -Wall -D__KERNEL__=0 -DSMP=0 -DMTSAFE=0 -DMAGBITLK=0 -DMAGSLABLK=1 -DMAGLK=0 -DMAGBITMAP=0 -DMEMTEST=1 -I../.. -I../../usr/lib -o memtest memtest.c ../mem/mag.c ../mem/slab.c -pthread
