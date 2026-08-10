#include <fancon/display.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

static SDL_Window *screen = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *signal = NULL;

static dword *signal_data = NULL;
static dword beam = 0;

static dword signal_width = 0;
static dword signal_height = 0;
static dword signal_full_width = 0;
static dword signal_area = 0;

static double target_time_step = 0.0;
static Uint64 frequency = 0;
static Uint64 frame_end = 0;
static double accumulator = 0;

static SDL_FRect visible_area = {0};

static DisplayType display_type = 0;
static const bool *keyboard = NULL;
static bool should_close = true;

static void __update() {
    Uint64 frame_start = SDL_GetPerformanceCounter();

    double frame_time = (double)(frame_start - frame_end) / frequency;
    accumulator += frame_time;
    frame_end = frame_start;

    SDL_Event event;
    while (SDL_PollEvent(&event)) switch (event.type) {
        case SDL_EVENT_QUIT: should_close = true; break;
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.key) {
                case SDLK_ESCAPE: should_close = true;
            }
    }
    keyboard = SDL_GetKeyboardState(NULL);
}

bool display_turn_on(const char *title, dword width, dword height, DisplayType type) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    if (!(screen = SDL_CreateWindow(title, width, height, 0))) {
        display_turn_off();
        return false;
    }

    if (!(renderer = SDL_CreateRenderer(screen, NULL))) {
        display_turn_off();
        return false;
    }
    SDL_SetRenderVSync(renderer, 1);

    target_time_step = 1.0 / 12;
    frequency = SDL_GetPerformanceFrequency();
    frame_end = SDL_GetPerformanceCounter();

    display_type = type;
    should_close = false;

    __update();

    return true;
}
void display_turn_off() {
    if (signal_data) free(signal_data);
    if (signal) SDL_DestroyTexture(signal);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (screen) SDL_DestroyWindow(screen);
    if (SDL_WasInit(SDL_INIT_VIDEO)) SDL_Quit();
}

bool display_should_close() { return should_close; }
bool display_is_key_pressed(DisplayKey key) { return keyboard ? keyboard[key] : false; }
bool display_is_frame_active() { return accumulator >= target_time_step; }

bool display_set_signal_size(word width, word height, word hblank, word vblank) {
    if (screen) {
        dword sw, sh;
        if (display_type == DISPLAY_TYPE_LCD) {
            sw = width;
            sh = height;
        } else {
            sw = width + hblank;
            sh = height + vblank;
        }
    
        SDL_Texture *new_signal = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            sw, sh
        );
        if (!new_signal) return false;
    
        dword size = sw * sh * sizeof(dword);
        dword *new_signal_data = (dword*)malloc(size);
        if (!new_signal_data) {
            SDL_DestroyTexture(new_signal);
            return false;
        }
    
        if (signal) SDL_DestroyTexture(signal);
        signal = new_signal;
        SDL_SetTextureScaleMode(signal, SDL_SCALEMODE_NEAREST);
    
        if (signal_data) free(signal_data);
        signal_data = new_signal_data;
        memset(signal_data, 0, size);
         
        signal_width = width;
        signal_height = height;
        signal_full_width = sw;
        signal_area = sw * sh;
    
        visible_area.w = width;
        visible_area.h = height;
    
        return true;
    } else return false;
}

void display_draw_pixel(dword color) {
    if (signal) {
        signal_data[beam++] = color;
        if (beam >= signal_area) {
            beam -= signal_area;
            SDL_UpdateTexture(
                signal,
                NULL,
                signal_data,
                signal_full_width * sizeof(dword)
            );
            accumulator -= target_time_step;
        } 
    }
}

void display_draw_pixels(dword color, size_t count) {
    if (signal) {
        while (count--) {
            signal_data[beam++] = color;
            if (beam >= signal_area) {
                beam -= signal_area;
                SDL_UpdateTexture(
                    signal,
                    NULL,
                    signal_data,
                    signal_full_width * sizeof(dword)
                );
                accumulator -= target_time_step;
            } 
        }
    }
}

void display_update() {
    if (screen) {
        if (signal) SDL_RenderTexture(renderer, signal, &visible_area, NULL);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        __update();
    }
}