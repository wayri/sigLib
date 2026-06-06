CC = gcc
CFLAGS = -Wall -Wextra -O2

SRCS = main.c control.c sigmain.c
OBJS = $(SRCS:.c=.o)
TARGET = test_siglib

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
