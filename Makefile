CC := gcc
CFLAGS := -Wall -Werror -ggdb

ctk.o: ctk.c ctk.h
	$(CC) $(CFLAGS) -c -o $@ $<

tests: ctk.o
	cd t/ && make

clean:
	rm -f ctk.o
	cd t/ && make clean
