all: build run

build: main.cpp
	g++ main.cpp -o animation -lSDL2 -lm

run:
	./animation

clean:
	rm -f animation