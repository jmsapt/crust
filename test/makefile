CC := gcc
CFLAGS = -Wall -g -fno-omit-frame-pointer -O3 # -fsanitize=address 
SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=)

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< ../lib/$<

.PHONY: clean
clean:
	find . -type f -executable | xargs rm
