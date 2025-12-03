CC = g++

CCFLAGS = -fsanitize=address -fsanitize=undefined -Wall -Wextra -O2 -Iinclude 

.PHONY: all clean run

all: bin user

bin:
	mkdir -p bin

user: 
	$(CC) $(CCFLAGS) ./src/user/main.cpp ./src/user/network.cpp ./src/common/parser.cpp \
	./src/user/protocol.cpp  -o ./bin/user

clean: 
	rm -r bin

run: 
	./bin/user


