CC = g++

CCFLAGS = -Wall -Wextra -O2 -Iinclude 

.PHONY: all clean run

all: user server

user: ./src/user/main.cpp ./src/user/network.cpp ./src/common/parser.cpp \
	./src/user/protocol_client.cpp ./src/common/utils.cpp ./src/common/file.cpp
	$(CC) $(CCFLAGS) ./src/user/main.cpp ./src/user/network.cpp ./src/common/parser.cpp \
	./src/user/protocol_client.cpp ./src/common/utils.cpp ./src/common/file.cpp -o ./user

server: ./src/server/main.cpp ./src/server/protocol_server.cpp ./src/server/database.cpp \
	./src/server/check_server_requests.cpp ./src/server/database_utils.cpp ./src/common/parser.cpp \
	./src/common/utils.cpp ./src/common/file.cpp ./src/server/server.cpp
	$(CC) $(CCFLAGS) ./src/server/main.cpp ./src/server/protocol_server.cpp ./src/server/database.cpp \
	./src/server/check_server_requests.cpp ./src/server/database_utils.cpp ./src/common/parser.cpp \
	 ./src/common/utils.cpp ./src/common/file.cpp ./src/server/server.cpp -o ./server

clean: 
	rm -rf user server

run: 
	./user