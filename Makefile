CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
CSTD := -std=c11
CPPFLAGS += -D_GNU_SOURCE
LDLIBS += -pthread

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)

diskmoon: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

build/%.o: src/%.c $(wildcard src/*.h) | build
	$(CC) $(CPPFLAGS) $(CSTD) $(CFLAGS) -pthread -c -o $@ $<

build:
	mkdir -p build

check: diskmoon
	python3 tests/run.py

clean:
	rm -rf build diskmoon

.PHONY: check clean
