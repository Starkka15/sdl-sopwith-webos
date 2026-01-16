/*
 * SDL2 to SDL1.2 compatibility layer for webOS
 * Global variables and PDL initialization
 */

#include <SDL.h>
#include <string.h>

#ifdef WEBOS
#include <PDL.h>
#endif

/* Define SDL2 event structure here before including compat header */
typedef struct {
    int scancode;
    SDLKey sym;
    SDLMod mod;
    Uint16 unicode;
} SDL2_Keysym_t;

typedef struct {
    Uint32 type;
    SDL2_Keysym_t keysym;
    Uint8 state;
    Uint8 repeat;
} SDL2_KeyboardEvent_t;

typedef struct {
    Uint32 type;
    char text[32];
} SDL2_TextInputEvent_t;

typedef struct {
    Uint32 type;
    Uint8 event;
} SDL2_WindowEvent_t;

typedef struct {
    Uint32 type;
    Sint64 touchId;
    Sint64 fingerId;
    float x;
    float y;
    float dx;
    float dy;
    float pressure;
} SDL2_TouchFingerEvent_t;

typedef struct {
    Uint32 type;
    Uint8 button;
    Uint8 state;
} SDL2_ControllerButtonEvent_t;

typedef struct {
    Uint32 type;
    Sint32 which;
} SDL2_ControllerDeviceEvent_t;

typedef struct {
    Uint32 type;
    union {
        SDL2_KeyboardEvent_t key;
        SDL2_TextInputEvent_t text;
        SDL2_WindowEvent_t window;
        SDL2_TouchFingerEvent_t tfinger;
        SDL2_ControllerButtonEvent_t cbutton;
        SDL2_ControllerDeviceEvent_t cdevice;
    };
} SDL2_Event_t;

/* Window event values */
#define SDL2_WINDOWEVENT        0x200
#define SDL2_WINDOWEVENT_CLOSE  14

SDL_Surface *sdl2_compat_screen = NULL;
SDL_Surface *sdl2_compat_render_target = NULL;
SDL_Surface *sdl2_compat_source_surface = NULL;
int sdl2_compat_text_input_active = 0;

/* Wrap SDL_Init to add PDL initialization */
int SDL2_Compat_Init(Uint32 flags) {
#ifdef WEBOS
    PDL_Init(0);
#endif
    return SDL_Init(flags);
}

/* Wrap SDL_Quit to add PDL cleanup */
void SDL2_Compat_Quit(void) {
    SDL_Quit();
#ifdef WEBOS
    PDL_Quit();
#endif
}

/* SDL2 touch event types */
#define SDL2_FINGERDOWN  0x110
#define SDL2_FINGERUP    0x111
#define SDL2_FINGERMOTION 0x112

/* Track mouse button state for motion events */
static int mouse_button_down = 0;

/* Wrap SDL_PollEvent to convert SDL1 events to SDL2 format */
int SDL2_PollEvent(void *ev) {
    SDL2_Event_t *event = (SDL2_Event_t *)ev;
    SDL_Event sdl1_event;
    float screen_w, screen_h;

    if (!SDL_PollEvent(&sdl1_event)) {
        return 0;
    }

    memset(event, 0, sizeof(*event));

    /* Get screen dimensions for normalizing touch coordinates */
    if (sdl2_compat_screen) {
        screen_w = (float)sdl2_compat_screen->w;
        screen_h = (float)sdl2_compat_screen->h;
    } else {
        screen_w = 1024.0f;
        screen_h = 768.0f;
    }

    switch (sdl1_event.type) {
    case SDL_KEYDOWN:
        event->type = SDL_KEYDOWN;
        event->key.keysym.scancode = sdl1_event.key.keysym.sym;
        event->key.keysym.sym = sdl1_event.key.keysym.sym;
        event->key.keysym.mod = sdl1_event.key.keysym.mod;
        event->key.state = sdl1_event.key.state;
        break;
    case SDL_KEYUP:
        event->type = SDL_KEYUP;
        event->key.keysym.scancode = sdl1_event.key.keysym.sym;
        event->key.keysym.sym = sdl1_event.key.keysym.sym;
        event->key.keysym.mod = sdl1_event.key.keysym.mod;
        event->key.state = sdl1_event.key.state;
        break;
    case SDL_MOUSEBUTTONDOWN:
        mouse_button_down = 1;
        event->type = SDL2_FINGERDOWN;
        event->tfinger.touchId = 1;
        event->tfinger.fingerId = 0;
        event->tfinger.x = sdl1_event.button.x / screen_w;
        event->tfinger.y = sdl1_event.button.y / screen_h;
        event->tfinger.pressure = 1.0f;
        break;
    case SDL_MOUSEBUTTONUP:
        mouse_button_down = 0;
        event->type = SDL2_FINGERUP;
        event->tfinger.touchId = 1;
        event->tfinger.fingerId = 0;
        event->tfinger.x = sdl1_event.button.x / screen_w;
        event->tfinger.y = sdl1_event.button.y / screen_h;
        event->tfinger.pressure = 0.0f;
        break;
    case SDL_MOUSEMOTION:
        if (mouse_button_down) {
            event->type = SDL2_FINGERMOTION;
            event->tfinger.touchId = 1;
            event->tfinger.fingerId = 0;
            event->tfinger.x = sdl1_event.motion.x / screen_w;
            event->tfinger.y = sdl1_event.motion.y / screen_h;
            event->tfinger.dx = sdl1_event.motion.xrel / screen_w;
            event->tfinger.dy = sdl1_event.motion.yrel / screen_h;
            event->tfinger.pressure = 1.0f;
        } else {
            event->type = sdl1_event.type;
        }
        break;
    case SDL_QUIT:
        event->type = SDL2_WINDOWEVENT;
        event->window.event = SDL2_WINDOWEVENT_CLOSE;
        break;
    case SDL_VIDEOEXPOSE:
        event->type = SDL2_WINDOWEVENT;
        event->window.event = 0;
        break;
    default:
        event->type = sdl1_event.type;
        break;
    }
    return 1;
}
