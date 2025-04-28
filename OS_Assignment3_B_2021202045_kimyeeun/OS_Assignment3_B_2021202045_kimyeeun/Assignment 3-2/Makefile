
CC = gcc
CFLAGS = -Wall -pthread


FORK_SRC = fork.c
THREAD_SRC = thread.c
NUMGEN_SRC = numgen.c

FORK_EXEC = fork
THREAD_EXEC = thread
NUMGEN_EXEC = numgen


all: $(FORK_EXEC) $(THREAD_EXEC) $(NUMGEN_EXEC)

$(FORK_EXEC): $(FORK_SRC)
	$(CC) $(CFLAGS) -o $(FORK_EXEC) $(FORK_SRC)

$(THREAD_EXEC): $(THREAD_SRC)
	$(CC) $(CFLAGS) -o $(THREAD_EXEC) $(THREAD_SRC)

$(NUMGEN_EXEC): $(NUMGEN_SRC)
	$(CC) $(CFLAGS) -o $(NUMGEN_EXEC) $(NUMGEN_SRC)

# Clean up generated files
clean:
	rm -f $(FORK_EXEC) $(THREAD_EXEC) $(NUMGEN_EXEC)

