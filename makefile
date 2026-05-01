CC=gcc
CFLAGS=-Wall -O2 
NAME=xmscan 
INSTALL_PATH=/usr/bin/$(NAME)
SOURCE_FILES=./src/main.c ./src/memory.c ./src/pattern.c

.PHONY: all install uninstall clean

all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) $(SOURCE_FILES) -o $(NAME)

install:
	cp $(NAME) $(INSTALL_PATH)

uninstall:
	rm $(INSTALL_PATH)

clean:
	rm -f $(NAME)
