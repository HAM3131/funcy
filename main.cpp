#include <stdio.h>
#include <set>
#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <queue>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define SIMULATION_PERIOD 8

struct CompareSDLPoint {
    bool operator()(const SDL_Point& a, const SDL_Point& b) const {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        return a.y < b.y;
    }
};

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

void DrawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    // Initialize variables for Bresenham's line algorithm
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    // Calculate offsets for thickness
    int offsetX = 0;
    int offsetY = 0;
    if (dx != 0) {
        offsetX = (thickness - 1) * sqrt(1 + (dy * dy) / (dx * dx));
    }
    if (dy != 0) {
        offsetY = (thickness - 1) * sqrt(1 + (dx * dx) / (dy * dy));
    }

    // Loop to draw the line
    while (true) {
        // Draw central rectangle with full alpha (255)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect;
        rect.x = x1 - thickness / 2;
        rect.y = y1 - thickness / 2;
        rect.w = thickness;
        rect.h = thickness;
        SDL_RenderFillRect(renderer, &rect);

        // Draw flanking rectangles with lower alpha values (128 and 64)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
        SDL_Rect rect1 = rect;
        rect1.x = rect.x - 1;
        SDL_RenderFillRect(renderer, &rect1);
        
        SDL_Rect rect2 = rect;
        rect2.x = rect.x + 1;
        SDL_RenderFillRect(renderer, &rect2);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 64);
        SDL_Rect rect3 = rect;
        rect3.x = rect.x - 2;
        SDL_RenderFillRect(renderer, &rect3);

        SDL_Rect rect4 = rect;
        rect4.x = rect.x + 2;
        SDL_RenderFillRect(renderer, &rect4);

        // Check for end of line
        if (x1 == x2 && y1 == y2) {
            break;
        }

        // Bresenham's line algorithm
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
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

SDL_Point drawFourier(SDL_Renderer *renderer, int *winDim, float ampArray[], float offsets[], int ampDegree, double t) {
    int colors = 3;
    float purpleness = 255.0/(colors-1);
    float prev_x = winDim[0]/2.0;
    float prev_y = winDim[1]/2.0;
    for (int i = 0; i < ampDegree; i++) {
        float x = prev_x + ampArray[i] * cos((i+1)*t+offsets[i]);
        float y = prev_y + ampArray[i] * (-sin((i+1)*t+offsets[i]));
        int color[] = {255-(int)(i%colors * purpleness),0,(int)(i%colors * purpleness),255};
        SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
        drawArrow(renderer, (int)prev_x, (int)prev_y, (int)x, (int)y);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 90);
        drawCircle(renderer, prev_x, prev_y, ampArray[i]);
        prev_x = x;
        prev_y = y;
    }

    return SDL_Point {(int)prev_x, (int)prev_y};
}

void drawLinesFromQueue(SDL_Renderer *renderer, std::queue<SDL_Point> pointQueue) {
    std::queue<SDL_Point> queueCopy = pointQueue;
    SDL_Point currentPoint;
    SDL_Point lastPoint;
    if (!queueCopy.empty()) {
        lastPoint = queueCopy.front();
        queueCopy.pop();
    }
    while (!queueCopy.empty()) {
        currentPoint = queueCopy.front();
        queueCopy.pop();
        DrawThickLine(renderer, currentPoint.x, currentPoint.y, lastPoint.x, lastPoint.y, 5);
        // SDL_RenderDrawLine(renderer, currentPoint.x, currentPoint.y, lastPoint.x, lastPoint.y);
        lastPoint = currentPoint;
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
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    int quit = 0;
    SDL_Event event;
    // Declare height and width variables
    int winDim[2] = {SCREEN_WIDTH, SCREEN_HEIGHT};

    // std::vector<SDL_Point> pointsVector = {};
    std::queue<SDL_Point> pointsQueue = {};
    SDL_Point next_point;

    // Declare timekeeping variables
    double time = 0;
    Uint32 prev_time = SDL_GetTicks();
    Uint32 now_time;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            // Handle quit event
            if (event.type == SDL_QUIT) {
                quit = 1;
            }

            // Handle window resize event
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                // You can handle the new window size here
                winDim[0] = event.window.data1;
                winDim[1] = event.window.data2;
            }
        }
        // Find the time passed since last iteration
        now_time = SDL_GetTicks();
        time += (((double)(now_time - prev_time)) / 1000.0f);
        prev_time = now_time;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set color to black
        SDL_RenderClear(renderer);

        // drawSineWave(renderer, phase);
        float amps[] = {200.0, 150.0, -125.0, 100.0, -75.0};
        float offsets[] = {1.0,0.5,23.2, 293.234, 0.0};
        next_point = drawFourier(renderer, winDim, amps, offsets, 5, time * 2.0 * M_PI / SIMULATION_PERIOD);
        pointsQueue.push(next_point);
        if (time > SIMULATION_PERIOD/2) {
            pointsQueue.pop();
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
        drawLinesFromQueue(renderer, pointsQueue);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // Delay to limit the frame rate
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
