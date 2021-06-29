CC := gcc
CFLAGS := -Wall -Werror -ggdb

OBJECTS := ctk.o
HEADERS := $(OBJECTS:.o=.h)

all: ctk.a

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

ctk.a: $(OBJECTS)
	ar -crs $@ $(OBJECTS)

tests: ctk.o
	cd t/ && make

clean:
	rm -f ctk.o
	cd t/ && make clean
