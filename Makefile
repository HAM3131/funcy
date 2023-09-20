all: oscillating_sine_wave

oscillating_sine_wave: main.c
	gcc main.c -o oscillating_sine_wave -lSDL2 -lm

clean:
	rm -f oscillating_sine_wave
