CC = gcc
CFLAGS = -Wall -Wextra -std=c11 $(shell pkg-config --cflags allegro-5 allegro_primitives-5 allegro_image-5)
LIBS = $(shell pkg-config --libs allegro-5 allegro_primitives-5 allegro_image-5)
SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = game

.PHONY: all clean run
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
