CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
CSTD := -std=c11
CPPFLAGS += -D_GNU_SOURCE
LDLIBS += -pthread
PREFIX ?= /usr/local

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)

diskmoon: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

build/%.o: src/%.c $(wildcard src/*.h) | build
	$(CC) $(CPPFLAGS) $(CSTD) $(CFLAGS) -pthread -c -o $@ $<

build:
	mkdir -p build

install: diskmoon
	install -Dm755 diskmoon $(DESTDIR)$(PREFIX)/bin/diskmoon
	install -Dm644 man/diskmoon.1 $(DESTDIR)$(PREFIX)/share/man/man1/diskmoon.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/diskmoon $(DESTDIR)$(PREFIX)/share/man/man1/diskmoon.1

check: diskmoon
	python3 tests/run.py

clean:
	rm -rf build diskmoon

.PHONY: install uninstall check clean
