CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = psinfo
SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/process_info.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean