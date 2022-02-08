CC=gcc
CFLAGS=-g -std=c11
BINS=parse shell

.PHONY: all clean test

all: $(BINS)

$(BINS): %: %.o
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test:
	/usr/bin/env python3 test_parse.py

clean:
	rm -f $(BINS)
	rm -f *.o
