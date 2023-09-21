#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include <time.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define SIMULATION_PERIOD 1

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

void drawArrow(SDL_Renderer *renderer, int x1, int y1, int x2, int y2) {
    // Draw the line segment from (x1, y1) to (x2, y2)
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

    // Calculate angle of the line
    float angle = atan2(y2 - y1, x2 - x1);

    // Calculate coordinates for the arrowhead
    int arrowLength = 20;
    float arrowAngle = M_PI / 6;

    int x3 = x2 - arrowLength * cos(angle - arrowAngle);
    int y3 = y2 - arrowLength * sin(angle - arrowAngle);
    int x4 = x2 - arrowLength * cos(angle + arrowAngle);
    int y4 = y2 - arrowLength * sin(angle + arrowAngle);

    // Draw the arrowhead
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x2, y2, x4, y4);
}

void drawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius) {
    
    if (radius < 0){
        radius *= -1;
    }

    for (int x = -radius; x <= radius; ++x) {
        int y = static_cast<int>(std::sqrt(radius * radius - x * x));
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
    }

    for (int y = -radius; y <= radius; ++y) {
        int x = static_cast<int>(std::sqrt(radius * radius - y * y));
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
    }
}

void drawFourier(SDL_Renderer *renderer, float ampArray[], float offsets[], int ampDegree, double t) {
    int colors = 3;
    float purpleness = 255.0/(colors-1);
    int prev_x = SCREEN_WIDTH/2;
    int prev_y = SCREEN_HEIGHT/2;
    for (int i = 0; i < ampDegree; i++) {
        int x = prev_x + ampArray[i] * cos((i+1)*t+offsets[i]);
        int y = prev_y + ampArray[i] * (-sin((i+1)*t+offsets[i]));
        int color[] = {255-(int)(i%colors * purpleness),0,(int)(i%colors * purpleness),255};
        SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
        drawArrow(renderer, prev_x, prev_y, x, y);
        SDL_SetRenderDrawColor(renderer, 0, 125+(int)(color[2]/2), 0, 10);
        drawCircle(renderer, prev_x, prev_y, ampArray[i]);
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
    double time = 0;

    Uint32 prev_time = SDL_GetTicks();
    Uint32 now_time;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        // Find the time passed since last iteration
        now_time = SDL_GetTicks();
        time += (((double)(now_time - prev_time)) / 1000.0f);
        prev_time = now_time;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set color to black
        SDL_RenderClear(renderer);

        //drawSineWave(renderer, phase);
        float amps[] = {200.0, 150.0, -125.0, 100.0, -75.0};
        float offsets[] = {1.0,0.5,23.2, 293.234, 0.0};
        drawFourier(renderer, amps, offsets, 5, time / SIMULATION_PERIOD);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // Delay to limit the frame rate
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
