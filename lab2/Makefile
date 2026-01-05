CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = myls

# default
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) *.o

.PHONY: clean