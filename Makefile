.PHONY: all clean run

all: user

user: 
	g++ user.cpp -o user

clean: 
	rm -f user

run: 
	./user


