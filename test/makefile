CC := gcc
CFLAGS = -Wall -g -fno-omit-frame-pointer -ggdb3 -fsanitize=address 
SRCS := $(wildcard *.c)
LIB  := $(wildcard ../lib/*.c)
BINS := $(SRCS:.c=)

all: $(BINS)

%: %.c ../lib/%.c
	$(CC) $(CFLAGS) -o $@ $< ../lib/$<

.PHONY: clean
clean:
	find . -type f -executable | xargs rm
