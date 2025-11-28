CC = g++
CCFLAGS = -fsanitize=address -fsanitize=undefined -Wall -Wextra -O2
.PHONY: all clean run

all: user

user: 
	$(CC) $(CCFLAGS) user.cpp -o user

clean: 
	rm -f user

run: 
	./user


