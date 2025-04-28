CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = page_replacement_simulator
SRC = page_replacement_simulator.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
