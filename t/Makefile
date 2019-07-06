CC := gcc
CFLAGS := -Wall -Werror -ggdb

LDFLAGS=-lncursesw

TESTS =
TESTS += area_t
TESTS += area_expand_x_t
TESTS += area_expand_y_t
TESTS += menu_t
TESTS += hbox_t
TESTS += vbox_t
TESTS += subwin_t
TESTS += complex_t
TESTS += vrule_t

all: $(TESTS) ../ctk.o

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ../ctk.o $<

clean:
	rm -f $(TESTS)
