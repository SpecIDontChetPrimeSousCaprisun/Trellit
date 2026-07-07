CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lcurl -lcjson

SRCS = src/main.c \
			 src/help.c \
			 src/init.c \
			 src/config.c \
			 src/list.c \
			 src/clone.c \
			 src/trello.c \
			 src/helper.c \
			 src/ls.c
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

test: $(OBJS)
	$(CC) $(CFLAGS) -o trellit $(OBJS) $(LDFLAGS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -f trellit
	rm -rf build

.PHONY: clean test
