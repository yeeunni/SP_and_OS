CC = gcc

CFLAGS = -Wall -Wextra -g

TARGET = fat

SRCS = fat.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) create example.txt

clean:
	rm -f $(OBJS) $(TARGET)

clean-all: clean
	rm -f fs_state.dat

