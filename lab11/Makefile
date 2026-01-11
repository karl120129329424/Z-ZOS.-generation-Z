CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = lab11

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)

.PHONY: clean