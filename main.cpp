#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Function to draw an oscillating sine wave
void drawSineWave(SDL_Renderer *renderer, float phase) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color to red

    int prev_x = 0;
    int prev_y = (int)(SCREEN_HEIGHT / 2 * (1 - sin(4 * M_PI * prev_x / SCREEN_WIDTH + phase)));

    for (int x = 1; x < SCREEN_WIDTH; x++) {
        // Compute the y-coordinate of the sine wave
        int y = (int)(SCREEN_HEIGHT / 2 * (1 - sin(4 * M_PI * x / SCREEN_WIDTH + phase)));

        // Draw a line from the previous point to the current point
        SDL_RenderDrawLine(renderer, prev_x, prev_y, x, y);

        prev_x = x;
        prev_y = y;
    }
}


void drawCircle(SDL_Renderer *renderer, int xIn, int yIn, int radius, int colors[]) {
    SDL_SetRenderDrawColor(renderer, colors[0], colors[1], colors[2], 255); // White circle


    for (int x = -radius; x <= radius; ++x) {
        int y = static_cast<int>(std::sqrt(radius * radius - x * x));
        SDL_RenderDrawPoint(renderer, xIn + x, yIn + y);
        SDL_RenderDrawPoint(renderer, xIn + x, yIn - y);
    }
}

void drawFourier(SDL_Renderer *renderer, float ampArray[], float offsets[], int ampDegree, float t) {
    float purpleness = 255.0/ampDegree;
    int prev_x = SCREEN_WIDTH/2;
    int prev_y = SCREEN_HEIGHT/2;
    for (int i = 0; i < ampDegree; i++) {
        int x = prev_x + ampArray[i] * cos((i+1)*t+offsets[i]);
        int y = prev_y + ampArray[i] * (-sin((i+1)*t+offsets[i]));
        SDL_SetRenderDrawColor(renderer, 255, 0, (int)(i * purpleness), 255);
        SDL_RenderDrawLine(renderer, prev_x, prev_y, x, y);
        int colorsIn[] = {255,255,255};
        drawCircle(renderer, prev_x, prev_y, ampArray[i], colorsIn);
        prev_x = x;
        prev_y = y;
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

    // Create an application window
    SDL_Window *window = SDL_CreateWindow(
        "Oscillating Sine Wave",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    
    int quit = 0;
    SDL_Event event;
    float time = 0;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set color to black
        SDL_RenderClear(renderer);

        //drawSineWave(renderer, phase);
        float amps[] = {200.0, 150.0, 125.0, 100.0, 75.0};
        float offsets[] = {1.0,0.5,23.2, 293.234, 0.0};
        drawFourier(renderer, amps, offsets, 5, time);

        SDL_RenderPresent(renderer);

        time += 0.01;  // Increment the phase to make the sine wave oscillate
        SDL_Delay(16);  // Delay to limit the frame rate
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
