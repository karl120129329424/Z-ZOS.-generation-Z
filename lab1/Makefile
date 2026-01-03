# lab1/Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Собираем ОБЕ утилиты по умолчанию
all: mycat mygrep

# Правило для mycat
mycat: mycat.c
	$(CC) -o $@ $< $(CFLAGS)

# Правило для mygrep
mygrep: mygrep.c
	$(CC) -o $@ $< $(CFLAGS)

# Чистим ОБА бинарника
clean:
	rm -f mycat mygrep

.PHONY: all clean