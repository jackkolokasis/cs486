CC 		= gcc
CFLAGS 	= -g -Wall
LDFLAGS = -lpthread -lm
RM 		= rm -rf
TARGET 	= phase1

.PHONY: clean distclean

all: phase1

phase1: singleLinkedList.o main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

singleLinkedList.o: singleLinkedList.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

clean:
	$(RM) *.o

distclean:
	$(RM) phase1

