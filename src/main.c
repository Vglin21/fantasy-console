#include <SDL3/SDL.h>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;

    SDL_Window *window;
    if (!(window = SDL_CreateWindow("It's Bad", 640, 480, 0))) {
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) switch (event.type) {
            case SDL_EVENT_QUIT: running = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}