CC := gcc
CFLAGS := -Wall -Werror -ggdb

LDFLAGS=-lncursesw

ctk.o: ctk.c ctk.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ctk.o
