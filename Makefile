CC = g++

CCFLAGS = -fsanitize=address -fsanitize=undefined -Wall -Wextra -O2 -Iinclude 

.PHONY: all clean run

all: bin user

bin:
	mkdir -p bin

user: 
	$(CC) $(CCFLAGS) ./src/user/main.cpp ./src/user/network.cpp ./src/common/parser.cpp \
	./src/user/protocol.cpp ./src/common/utils.cpp ./src/common/file.cpp  -o ./user

clean: 
	rm -r user

run: 
	./user