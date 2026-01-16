/*
 * SDL2 to SDL1.2 compatibility layer for webOS
 */

#ifndef SDL2_COMPAT_H
#define SDL2_COMPAT_H

#include <SDL.h>
#include <string.h>
#include <stdlib.h>

/* SDL2 type mappings */
typedef SDL_Surface SDL_Window;
typedef SDL_Surface SDL_Renderer;
typedef SDL_Surface SDL_Texture;
typedef SDLKey SDL_Keycode;

typedef struct {
    int max_texture_width;
    int max_texture_height;
} SDL_RendererInfo;

typedef struct {
    int w;
    int h;
    int refresh_rate;
    Uint32 format;
} SDL_DisplayMode;

/* SDL2 Keysym - add scancode field */
typedef struct {
    int scancode;
    SDLKey sym;
    SDLMod mod;
    Uint16 unicode;
} SDL2_Keysym;

/* SDL2 event structures */
typedef struct {
    Uint32 type;
    SDL2_Keysym keysym;
    Uint8 state;
    Uint8 repeat;
} SDL2_KeyboardEvent;

typedef struct {
    Uint32 type;
    char text[32];
} SDL2_TextInputEvent;

typedef struct {
    Uint32 type;
    Uint8 event;
} SDL2_WindowEvent;

typedef struct {
    Uint32 type;
    Sint64 touchId;
    Sint64 fingerId;
    float x;
    float y;
    float dx;
    float dy;
    float pressure;
} SDL2_TouchFingerEvent;

typedef struct {
    Uint32 type;
    Sint32 which;
    Uint8 button;
    Uint8 state;
} SDL2_ControllerButtonEvent;

typedef struct {
    Uint32 type;
    Sint32 which;
} SDL2_ControllerDeviceEvent;

typedef struct {
    Uint32 type;
    union {
        SDL2_KeyboardEvent key;
        SDL2_TextInputEvent text;
        SDL2_WindowEvent window;
        SDL2_TouchFingerEvent tfinger;
        SDL2_ControllerButtonEvent cbutton;
        SDL2_ControllerDeviceEvent cdevice;
    };
} SDL2_Event;

/* Override SDL types */
#define SDL_Keysym SDL2_Keysym
#define SDL_KeyboardEvent SDL2_KeyboardEvent
#define SDL_TextInputEvent SDL2_TextInputEvent
#define SDL_WindowEvent SDL2_WindowEvent
#define SDL_TouchFingerEvent SDL2_TouchFingerEvent
#define SDL_ControllerButtonEvent SDL2_ControllerButtonEvent
#define SDL_ControllerDeviceEvent SDL2_ControllerDeviceEvent
#define SDL_Event SDL2_Event

/* SDL2 window flags */
#define SDL_WINDOW_RESIZABLE        0x00000020
#define SDL_WINDOW_ALLOW_HIGHDPI    0x00002000
#define SDL_WINDOW_FULLSCREEN_DESKTOP 0x00001001
#define SDL_WINDOWPOS_CENTERED      0x2FFF0000

/* SDL2 renderer flags */
#define SDL_RENDERER_PRESENTVSYNC   0x00000004
#define SDL_TEXTUREACCESS_TARGET    2
#define SDL_TEXTUREACCESS_STREAMING 1

/* SDL2 hints */
#define SDL_HINT_RENDER_SCALE_QUALITY "SDL_RENDER_SCALE_QUALITY"

/* SDL2 window events */
#define SDL_WINDOWEVENT             0x200
#define SDL_WINDOWEVENT_CLOSE       14

/* SDL2 event types - use values that don't conflict with SDL1 */
#define SDL2_TEXTINPUT              0x103
#define SDL2_FINGERDOWN             0x110
#define SDL2_FINGERUP               0x111
#define SDL2_FINGERMOTION           0x112
#define SDL2_CONTROLLERBUTTONDOWN   0x120
#define SDL2_CONTROLLERBUTTONUP     0x121
#define SDL2_CONTROLLERDEVICEADDED  0x122
#define SDL2_CONTROLLERDEVICEREMOVED 0x123

/* Map SDL2 event names to our values */
#define SDL_TEXTINPUT               SDL2_TEXTINPUT
#define SDL_FINGERDOWN              SDL2_FINGERDOWN
#define SDL_FINGERUP                SDL2_FINGERUP
#define SDL_FINGERMOTION            SDL2_FINGERMOTION
#define SDL_CONTROLLERBUTTONDOWN    SDL2_CONTROLLERBUTTONDOWN
#define SDL_CONTROLLERBUTTONUP      SDL2_CONTROLLERBUTTONUP
#define SDL_CONTROLLERDEVICEADDED   SDL2_CONTROLLERDEVICEADDED
#define SDL_CONTROLLERDEVICEREMOVED SDL2_CONTROLLERDEVICEREMOVED

/* SDL2 message box */
#define SDL_MESSAGEBOX_ERROR        0x00000010

/* SDL2 scancodes */
typedef int SDL_Scancode;
#define SDL_SCANCODE_UNKNOWN    0
#define SDL_SCANCODE_COMMA      SDLK_COMMA
#define SDL_SCANCODE_SLASH      SDLK_SLASH
#define SDL_SCANCODE_PERIOD     SDLK_PERIOD
#define SDL_SCANCODE_B          SDLK_b
#define SDL_SCANCODE_SPACE      SDLK_SPACE
#define SDL_SCANCODE_H          SDLK_h
#define SDL_SCANCODE_V          SDLK_v
#define SDL_SCANCODE_C          SDLK_c
#define SDL_SCANCODE_X          SDLK_x
#define SDL_SCANCODE_Z          SDLK_z
#define SDL_SCANCODE_S          SDLK_s
#define SDL_SCANCODE_RETURN     SDLK_RETURN
#define SDL_SCANCODE_ESCAPE     SDLK_ESCAPE
#define SDL_SCANCODE_UP         SDLK_UP
#define SDL_SCANCODE_DOWN       SDLK_DOWN
#define SDL_SCANCODE_LEFT       SDLK_LEFT
#define SDL_SCANCODE_RIGHT      SDLK_RIGHT
#define SDL_SCANCODE_BACKSPACE  SDLK_BACKSPACE
#define SDL_SCANCODE_TAB        SDLK_TAB

/* Global screen surface */
extern SDL_Surface *sdl2_compat_screen;
extern int sdl2_compat_text_input_active;

/* PDL-wrapped init/quit */
extern int SDL2_Compat_Init(Uint32 flags);
extern void SDL2_Compat_Quit(void);
#define SDL_Init SDL2_Compat_Init
#undef SDL_Quit
#define SDL_Quit SDL2_Compat_Quit

/* Function implementations */
static inline SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
    Uint32 sdl_flags = SDL_SWSURFACE;
    (void)x; (void)y; (void)flags;
    /* webOS always runs fullscreen */
    sdl_flags |= SDL_FULLSCREEN;
    sdl2_compat_screen = SDL_SetVideoMode(w, h, 0, sdl_flags);
    if (sdl2_compat_screen) {
        SDL_WM_SetCaption(title, title);
    }
    return sdl2_compat_screen;
}

static inline SDL_Renderer *SDL_CreateRenderer(SDL_Window *window, int index, Uint32 flags) {
    (void)index; (void)flags;
    return window;
}

static inline SDL_Texture *SDL_CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h) {
    (void)renderer; (void)format; (void)access;
    /* Use screen format for compatibility */
    if (sdl2_compat_screen && sdl2_compat_screen->format) {
        return SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
            sdl2_compat_screen->format->BitsPerPixel,
            sdl2_compat_screen->format->Rmask,
            sdl2_compat_screen->format->Gmask,
            sdl2_compat_screen->format->Bmask,
            sdl2_compat_screen->format->Amask);
    }
    return SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
}

static inline int SDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch) {
    (void)rect;
    if (texture && texture->format && pixels) {
        int y;
        int bpp = texture->format->BytesPerPixel;
        Uint8 *dst = (Uint8*)texture->pixels;
        const Uint8 *src = (const Uint8*)pixels;
        for (y = 0; y < texture->h; y++) {
            memcpy(dst, src, texture->w * bpp);
            dst += texture->pitch;
            src += pitch;
        }
    }
    return 0;
}

/* Track the source pixels for direct blitting */
extern SDL_Surface *sdl2_compat_source_surface;

static inline int SDL_RenderClear(SDL_Renderer *renderer) {
    (void)renderer;
    return 0;
}

static inline int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect) {
    (void)renderer; (void)srcrect; (void)dstrect;
    /* Just remember the first texture (the one with actual pixel data) */
    if (!sdl2_compat_source_surface) {
        sdl2_compat_source_surface = texture;
    }
    return 0;
}

static inline void SDL_RenderPresent(SDL_Renderer *renderer) {
    (void)renderer;
    if (!sdl2_compat_screen) return;

    if (sdl2_compat_source_surface) {
        SDL_Rect src_rect = {0, 0, sdl2_compat_source_surface->w, sdl2_compat_source_surface->h};
        SDL_Rect dst_rect = {0, 0, sdl2_compat_screen->w, sdl2_compat_screen->h};

        /* Scale source to fill screen */
        SDL_SoftStretch(sdl2_compat_source_surface, &src_rect, sdl2_compat_screen, &dst_rect);
        sdl2_compat_source_surface = NULL;
    }

    SDL_Flip(sdl2_compat_screen);
}

static inline int SDL_SetRenderTarget(SDL_Renderer *renderer, SDL_Texture *texture) {
    (void)renderer; (void)texture;
    return 0;
}

static inline void SDL_DestroyTexture(SDL_Texture *texture) {
    if (texture && texture != sdl2_compat_screen) {
        SDL_FreeSurface(texture);
    }
}

static inline void SDL_DestroyRenderer(SDL_Renderer *renderer) {
    (void)renderer;
}

static inline void SDL_DestroyWindow(SDL_Window *window) {
    (void)window;
    sdl2_compat_screen = NULL;
}

static inline int SDL_GetRendererInfo(SDL_Renderer *renderer, SDL_RendererInfo *info) {
    (void)renderer;
    info->max_texture_width = 2048;
    info->max_texture_height = 2048;
    return 0;
}

static inline int SDL_GetRendererOutputSize(SDL_Renderer *renderer, int *w, int *h) {
    if (renderer) {
        *w = renderer->w;
        *h = renderer->h;
    }
    return 0;
}

static inline int SDL_GetDesktopDisplayMode(int displayIndex, SDL_DisplayMode *mode) {
    const SDL_VideoInfo *info;
    (void)displayIndex;
    info = SDL_GetVideoInfo();
    if (info) {
        mode->w = info->current_w;
        mode->h = info->current_h;
        mode->refresh_rate = 60;
        mode->format = 0;
    }
    return 0;
}

static inline Uint32 SDL_GetWindowPixelFormat(SDL_Window *window) {
    (void)window;
    return 0;
}

static inline void SDL_SetWindowSize(SDL_Window *window, int w, int h) {
    (void)window; (void)w; (void)h;
}

static inline int SDL_SetWindowFullscreen(SDL_Window *window, Uint32 flags) {
    (void)window; (void)flags;
    return 0;
}

static inline void SDL_SetWindowIcon(SDL_Window *window, SDL_Surface *icon) {
    (void)window;
    SDL_WM_SetIcon(icon, NULL);
}

static inline SDL_bool SDL_SetHint(const char *name, const char *value) {
    (void)name; (void)value;
    return SDL_TRUE;
}

static inline void SDL_StopTextInput(void) {
    sdl2_compat_text_input_active = 0;
}

static inline void SDL_StartTextInput(void) {
    sdl2_compat_text_input_active = 1;
    SDL_EnableUNICODE(1);
}

static inline SDL_bool SDL_IsTextInputActive(void) {
    return sdl2_compat_text_input_active ? SDL_TRUE : SDL_FALSE;
}

static inline int SDL_RenderSetLogicalSize(SDL_Renderer *renderer, int w, int h) {
    (void)renderer; (void)w; (void)h;
    return 0;
}

static inline int SDL_SetRenderDrawColor(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    (void)renderer; (void)r; (void)g; (void)b; (void)a;
    return 0;
}

static inline int SDL_SetPaletteColors(SDL_Palette *palette, const SDL_Color *colors, int firstcolor, int ncolors) {
    int i;
    if (!palette || !colors) return -1;
    for (i = 0; i < ncolors && (firstcolor + i) < palette->ncolors; i++) {
        palette->colors[firstcolor + i] = colors[i];
    }
    return 0;
}

static inline int SDL_PixelFormatEnumToMasks(Uint32 format, int *bpp, Uint32 *Rmask, Uint32 *Gmask, Uint32 *Bmask, Uint32 *Amask) {
    (void)format;
    /* Use screen format for compatibility */
    if (sdl2_compat_screen && sdl2_compat_screen->format) {
        *bpp = sdl2_compat_screen->format->BitsPerPixel;
        *Rmask = sdl2_compat_screen->format->Rmask;
        *Gmask = sdl2_compat_screen->format->Gmask;
        *Bmask = sdl2_compat_screen->format->Bmask;
        *Amask = sdl2_compat_screen->format->Amask;
        return SDL_TRUE;
    }
    *bpp = 32;
    *Rmask = 0x00FF0000;
    *Gmask = 0x0000FF00;
    *Bmask = 0x000000FF;
    *Amask = 0xFF000000;
    return SDL_TRUE;
}

static inline int SDL_GetNumTouchDevices(void) {
    return 1; /* webOS TouchPad has touchscreen */
}

static inline Sint64 SDL_GetTouchDevice(int index) {
    (void)index;
    return 1; /* Return valid touch device ID */
}

static inline const char *SDL_GetScancodeName(int scancode) {
    return SDL_GetKeyName((SDLKey)scancode);
}

static inline int SDL_ShowSimpleMessageBox(Uint32 flags, const char *title, const char *message, SDL_Window *window) {
    (void)flags; (void)title; (void)window;
    fprintf(stderr, "%s\n", message);
    return -1;
}

static inline char *SDL_GetPrefPath(const char *org, const char *app) {
    char *path;
    const char *home;
    (void)org;
    home = getenv("HOME");
    if (!home) home = "/media/internal";
    path = (char*)malloc(256);
    if (path) {
        snprintf(path, 256, "%s/.%s/", home, app);
    }
    return path;
}

/* Wrap SDL_PollEvent - declared in sdl2_compat.c, uses void* for type compatibility */
extern int SDL2_PollEvent(void *event);
#define SDL_PollEvent(e) SDL2_PollEvent((void*)(e))

/* SDL2 Audio API compatibility */
typedef Uint32 SDL_AudioDeviceID;
#define SDL_AUDIO_ALLOW_FREQUENCY_CHANGE 0x00000001

static inline SDL_AudioDeviceID SDL_OpenAudioDevice(const char *device, int iscapture,
                                                    const SDL_AudioSpec *desired,
                                                    SDL_AudioSpec *obtained, int allowed_changes) {
    (void)device; (void)iscapture; (void)allowed_changes;
    if (SDL_OpenAudio((SDL_AudioSpec*)desired, obtained) < 0) {
        return 0;
    }
    return 1; /* Return dummy device ID */
}

static inline void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on) {
    (void)dev;
    SDL_PauseAudio(pause_on);
}

static inline void SDL_CloseAudioDevice(SDL_AudioDeviceID dev) {
    (void)dev;
    SDL_CloseAudio();
}

/* SDL2 Game Controller API - stub for webOS (no controllers) */
typedef void* SDL_GameController;
typedef Sint32 SDL_JoystickID;

#define SDL_INIT_GAMECONTROLLER 0

#define SDL_CONTROLLER_BUTTON_INVALID -1
#define SDL_CONTROLLER_BUTTON_A 0
#define SDL_CONTROLLER_BUTTON_B 1
#define SDL_CONTROLLER_BUTTON_X 2
#define SDL_CONTROLLER_BUTTON_Y 3
#define SDL_CONTROLLER_BUTTON_BACK 4
#define SDL_CONTROLLER_BUTTON_GUIDE 5
#define SDL_CONTROLLER_BUTTON_START 6
#define SDL_CONTROLLER_BUTTON_LEFTSTICK 7
#define SDL_CONTROLLER_BUTTON_RIGHTSTICK 8
#define SDL_CONTROLLER_BUTTON_LEFTSHOULDER 9
#define SDL_CONTROLLER_BUTTON_RIGHTSHOULDER 10
#define SDL_CONTROLLER_BUTTON_DPAD_UP 11
#define SDL_CONTROLLER_BUTTON_DPAD_DOWN 12
#define SDL_CONTROLLER_BUTTON_DPAD_LEFT 13
#define SDL_CONTROLLER_BUTTON_DPAD_RIGHT 14
#define SDL_CONTROLLER_BUTTON_MAX 15

/* SDL_NumJoysticks exists in SDL1, no need to redefine */
static inline int SDL_IsGameController(int joystick_index) { (void)joystick_index; return 0; }
static inline SDL_GameController *SDL_GameControllerOpen(int joystick_index) { (void)joystick_index; return NULL; }
static inline void SDL_GameControllerClose(SDL_GameController *controller) { (void)controller; }
static inline const char *SDL_GameControllerName(SDL_GameController *controller) { (void)controller; return ""; }
static inline const char *SDL_GameControllerGetStringForButton(int button) { (void)button; return ""; }
static inline SDL_Joystick *SDL_GameControllerGetJoystick(SDL_GameController *controller) { (void)controller; return NULL; }
static inline int SDL_JoystickInstanceID(SDL_Joystick *joystick) { (void)joystick; return -1; }
static inline SDL_GameController *SDL_GameControllerFromInstanceID(int joyid) { (void)joyid; return NULL; }

#endif /* SDL2_COMPAT_H */
