CC 		= gcc
CFLAGS 	= -g -Wall
LDFLAGS = -lpthread -lm
RM 		= rm -rf
TARGET 	= phase1

.PHONY: run clean distclean

all: phase1

phase1: singleLinkedList.o unboundedLockFreeQueue.o main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

singleLinkedList.o: singleLinkedList.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

unboundedLockFreeQueue.o: unboundedLockFreeQueue.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

run:
	./phase1 -n 2 > err 2>&1 > out

clean:
	$(RM) *.o err out

distclean:
	$(RM) phase1
