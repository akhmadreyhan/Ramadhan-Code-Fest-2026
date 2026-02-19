CC = clang
CFLAGS = -std=c99 -Wall -Wextra -O2 -Isrc -Ivendor
LIBS = -lcurl

SRC = \
	src/main.c \
	src/city.c \
	src/api.c \
	vendor/cJSON.c

OBJ = $(SRC:%.c=build/%.o)
BIN = sholat

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN)

PREFIX ?= /usr/local

install:
	install -m 0755 $(BIN) $(PREFIX)/bin/$(BIN)

.PHONY: all clean install