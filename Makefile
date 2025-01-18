.PHONY: all clean run report analyze

all: main

main: main.cpp performant_thread.h performant_thread.so Makefile
	g++ -std=c++11 -Wall -Wextra -Werror -O2 $< -L. -l:performant_thread.so -o $@ -lpthread

run: main
	./main

clean: Makefile
	rm main
