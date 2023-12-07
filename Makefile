
all: poke327

poke327: main.cpp
	g++ -o poke327 map.cpp game.cpp priorityQueue.cpp -lncurses main.cpp -Wall -Werror
clean:
	rm -f poke327 *~ *.o