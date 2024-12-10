# Target to build both prog and test_allocator
all : prog test_allocator

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -pthread  # Add -pthread to compiler flags

# Build the main program
prog : main.c allocator.c allocator.h
	$(CC) $(CFLAGS) -o $@ $^

# Build the test program
test_allocator : test_allocator.c allocator.c allocator.h
	$(CC) $(CFLAGS) -o $@ $^ -lcmocka -pthread  # Add -pthread to link with pthread

# Clean function
clean:
	rm -f prog test_allocator

# Run the tests
check :
	./test_allocator

# PHONY targets
.PHONY: clean check all
