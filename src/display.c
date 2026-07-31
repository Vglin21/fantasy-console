#include <fancon/display.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

struct Display {
    DisplayType type;

    SDL_Window *screen;
    SDL_Renderer *renderer;
    SDL_Texture *signal;
    dword *signal_data;
    dword beam;
    dword beam_crt;
    SignalSize signal_size;
    
    bool should_close;
};

Display *display_new(const char *title, dword width, dword height, DisplayType type) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return NULL;

    Display *display;
    if (!(display = (Display*)malloc(sizeof(Display)))) {
        SDL_Quit();
        return NULL;
    } else memset(display, 0, sizeof(Display));

    if (!(display->screen = SDL_CreateWindow(title, width, height, 0))) {
        display_destroy(&display);
        return NULL;
    }

    if (!(display->renderer = SDL_CreateRenderer(display->screen, NULL))) {
        display_destroy(&display);
        return NULL;
    }
    SDL_SetRenderVSync(display->renderer, 1);

    display->type = type;

    return display;
}
void display_destroy(Display **display) {
    if (display && *display) {
        Display *d = *display;
        if (d->signal) SDL_DestroyTexture(d->signal);
        if (d->renderer) SDL_DestroyRenderer(d->renderer);
        if (d->screen) SDL_DestroyWindow(d->screen);
        SDL_Quit();
        if (d->signal_data) free(d->signal_data);
        free(d);
        *display = NULL;
    }
}

bool display_should_close(Display *display) { return display ? display->should_close : true; }

bool display_set_signal_size(Display *display, word width, word height, word hblank, word vblank) {
    if (display) {
        SDL_Texture *new_signal = SDL_CreateTexture(
            display->renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            width, height
        );
        if (!new_signal) return false;

        dword size = width * height * sizeof(dword);
        dword *new_signal_data = (dword*)malloc(size);
        if (!new_signal_data) {
            SDL_DestroyTexture(new_signal);
            return false;
        }

        if (display->signal) SDL_DestroyTexture(display->signal);
        display->signal = new_signal;
        SDL_SetTextureScaleMode(display->signal, SDL_SCALEMODE_NEAREST);

        display->signal_data = new_signal_data;
        memset(display->signal_data, 0, size);
        
        SignalSize *signal = &display->signal_size;
        
        signal->width = width;
        signal->height = height;
        signal->area = width * height;
        if (display->type == DISPLAY_TYPE_CRT) {
            signal->hblank = hblank;
            signal->vblank = vblank;
            signal->full_width = width + hblank;
            signal->full_height = height + vblank;
            signal->full_area = signal->full_width * signal->full_height;
        }

        return true;
    } else return false;
}

void display_draw_pixel(Display *display, dword color) {
    if (display) {
        if (display->type == DISPLAY_TYPE_CRT) {
            dword bx = display->beam % display->signal_size.full_width;
            dword by = display->beam / display->signal_size.full_width;
            display->beam++;

            if (bx < display->signal_size.width && by < display->signal_size.height)
                display->signal_data[display->beam_crt++] = color;

            if (display->beam >= display->signal_size.full_area) {
                display->beam_crt -= display->signal_size.area;
                display->beam -= display->signal_size.full_area; 
            }
        } else {
            display->signal_data[display->beam++] = color;
                
            if (display->beam >= display->signal_size.area)
                display->beam -= display->signal_size.area; 
        }
    }
}

void display_draw_pixels(Display *display, dword color, size_t count) {}

void display_update(Display *display) {
    if (display) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) switch (event.type) {
            case SDL_EVENT_QUIT: display->should_close = true; break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_ESCAPE: display->should_close = true;
                }
        }
    
        SDL_RenderClear(display->renderer);
    
        if (display->signal) {
            SDL_UpdateTexture(
                display->signal,
                NULL,
                display->signal_data,
                display->signal_size.width * sizeof(dword)
            );
            SDL_RenderTexture(display->renderer, display->signal, NULL, NULL);
        }
        SDL_RenderPresent(display->renderer);
    }
}