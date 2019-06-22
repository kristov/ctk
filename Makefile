CC := gcc
CFLAGS := -Wall -Werror -ggdb

LDFLAGS=-lncursesw

ctk.o: ctk.c ctk.h
	$(CC) $(CFLAGS) -c -o $@ $<

test_ctk: test_ctk.c ctk.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ctk.o $<

clean:
	rm -f ctk.o test_ctk
