all : prog test_allocator

CC = gcc #compileur 
CFLAGS = -Wall -Wextra -Wpedantic #compileur options

#build le programme
prog : main.c allocator.c allocator.h
	$(CC) $(CFLAGS) -o $@ $^

#build les tests
test_allocator : test_allocator.c allocator.c allocator.h
	$(CC) $(CFLAGS) -o $@ $^ -lcmocka

#clean fonction
clean:
	rm -f prog test_allocator

#run les tests
check :
	./test_allocator

#PHONY
.PHONY: clean check all