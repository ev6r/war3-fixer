CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = war3-fixer.exe
OBJS = main.o config.o fixer.o winapi.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c config.h fixer.h winapi.h
config.o: config.c config.h
fixer.o: fixer.c fixer.h winapi.h
winapi.o: winapi.c winapi.h

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean