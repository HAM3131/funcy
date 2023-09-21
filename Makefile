all: build run

build: main.cpp
	gcc main.cpp -o animation -lSDL2 -lm

run:
	./animation

clean:
	rm -f animation
