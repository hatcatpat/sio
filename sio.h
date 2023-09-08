#define SIO_DEV
#ifdef SIO_DEV
#define SIO_IMPL
#define SIO_SDL
#endif

#ifndef SIO
#define SIO

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define PI 3.14159265358979323846
#define TAU (2 * PI)
#define SQRT_2 1.41421356237309504880

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int int16_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long int int64_t;
typedef unsigned long int uint64_t;
typedef unsigned int uint_t;

enum
{
    false,
    true
};
typedef uint8_t bool;

#define SIO_WHITE (0b111111)
#define SIO_BLACK (0b000000)
#define SIO_RED (0b110000)
#define SIO_GREEN (0b001100)
#define SIO_BLUE (0b000011)
#define SIO_INVIS (0b1000000)
#define SIO_IS_INVIS(c) (((c)&SIO_INVIS) != 0)
#define SIO_IS_VIS(c) (!SIO_IS_INVIS (c))

typedef struct
{
    int width, height;
    uint8_t *data;
} sio_canvas_t;

typedef struct
{
    bool go;
    sio_canvas_t screen, *canvas;
    uint8_t bg, recolor;
    int mouse[2];
    bool key[0xff], button[3], oldkey[0xff], oldbutton[3];
    void (*mousemove) (int x, int y);
    void (*keyup) (char key);
    void (*keydown) (char key);
    void (*buttonup) (uint8_t button);
    void (*buttondown) (uint8_t button);
} sio_t;

extern sio_t s;

int sio_init (int width, int height);
void sio_deinit ();
void sio_loop ();
bool sio_pressed (uint8_t key);
bool sio_buttoned (uint8_t button);
void sio_canvas_init (sio_canvas_t *canvas, int width, int height);
void sio_canvas_deinit (sio_canvas_t *canvas);
uint8_t sio_rgb (uint8_t r, uint8_t g, uint8_t b);
void sio_clear ();
void sio_pixel (int x, int y, uint8_t c);
void sio_bliti (uint8_t *data, int x, int y, int w, int h);
void sio_blit (uint8_t *data, int x, int y, int w, int h, int sw, int sh);
void sio_rectf (int x, int y, int w, int h, uint8_t c);
void sio_rect (int x, int y, int w, int h, int t, uint8_t c);
void sio_line (int sx, int sy, int ex, int ey, uint8_t c);
void sio_linecircle (int sx, int sy, int ex, int ey, int r, uint8_t c);
void sio_linerect (int sx, int sy, int ex, int ey, int w, int h, uint8_t c);
void sio_circle (int x, int y, int r, uint8_t c);
void sio_circlef (int x, int y, int r, uint8_t c);

int emu_init ();
void emu_deinit ();
void emu_loop ();
void emu_stretch (int width, int height);

#ifdef SIO_IMPL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sio_t s;

int
sio_init (int width, int height)
{
    memset (&s, 0, sizeof (sio_t));
    sio_canvas_init (&s.screen, width, height);
    s.canvas = &s.screen;
    s.go = true;
    s.recolor = 0b1111111;
    return emu_init ();
}

void
sio_deinit ()
{
    sio_canvas_deinit (&s.screen);
    emu_deinit ();
}

void
sio_cursor ()
{
    static uint8_t cursor[8 * 8] = {
        0x40, 0x0,  0x0,  0x0,  0x0, 0x0,  0x0,  0x0,  0x40, 0x0,  0x3f,
        0x3f, 0x3f, 0x3f, 0x3f, 0x0, 0x40, 0x0,  0x0,  0x0,  0x3f, 0x3f,
        0x3f, 0x0,  0x40, 0x0,  0x0, 0x3f, 0x3f, 0x3f, 0x3f, 0x0,  0x0,
        0x0,  0x3f, 0x3f, 0x3f, 0x0, 0x3f, 0x0,  0x0,  0x3f, 0x3f, 0x3f,
        0x0,  0x0,  0x3f, 0x0,  0x0, 0x0,  0x3f, 0x0,  0x0,  0x0,  0x0,
        0x0,  0x40, 0x0,  0x0,  0x0, 0x40, 0x40, 0x40, 0x40,
    };
    uint8_t old = s.recolor;
    if (s.button[0] || s.button[1] || s.button[2])
        s.recolor = 0b1101010;
    else
        s.recolor = 0b1111111;
    sio_bliti (cursor, s.mouse[0] - 8, s.mouse[1], 8, 8);
    s.recolor = old;
}

void
sio_loop ()
{
    memcpy (s.oldkey, s.key, 0xff);
    memcpy (s.oldbutton, s.button, 3);
    sio_cursor ();
    emu_loop ();
}

bool
sio_pressed (uint8_t key)
{
    return !s.oldkey[key] && s.key[key];
}

bool
sio_buttoned (uint8_t button)
{
    return !s.oldbutton[button] && s.button[button];
}

void
sio_canvas_init (sio_canvas_t *canvas, int width, int height)
{
    canvas->width = MAX (1, width), canvas->height = MAX (1, height);
    canvas->data = calloc (canvas->width * canvas->height, 1);
}

void
sio_canvas_deinit (sio_canvas_t *canvas)
{
    canvas->width = canvas->height = 0;
    if (canvas->data != NULL)
        free (canvas->data);
}

void
sio_clear ()
{
    memset (s.canvas->data, s.bg, s.canvas->width * s.canvas->height);
}

void
sio_pixel_raw (int x, int y, uint8_t c)
{
    s.canvas->data[x + y * s.canvas->width] = c & s.recolor;
}

void
sio_pixel (int x, int y, uint8_t c)
{
    if (0 <= x && x < s.canvas->width && 0 <= y && y < s.canvas->height)
        sio_pixel_raw (x, y, c);
}

void
sio_bliti (uint8_t *data, int x, int y, int w, int h)
{
    if (x > -w && y > -h && x < s.canvas->width && y < s.canvas->height)
        {
            int i, j;
            uint8_t c;
            for (i = MAX (-x, 0); i < MIN (w, s.canvas->width - x); ++i)
                for (j = MAX (-y, 0); j < MIN (h, s.canvas->height - y); ++j)
                    {
                        c = data[i + j * w];
                        if (SIO_IS_VIS (c))
                            sio_pixel_raw (x + i, y + j, c);
                    }
        }
}

void
sio_blit (uint8_t *data, int x, int y, int w, int h, int sw, int sh)
{
    if (x > -sw && y > -sh && x < s.canvas->width && y < s.canvas->height)
        {
            int i, j;
            uint8_t c;
            for (i = MAX (-x, 0); i < MIN (sw, s.canvas->width - x); ++i)
                for (j = MAX (-y, 0); j < MIN (sh, s.canvas->height - y); ++j)
                    {
                        c = data[(i * w / sw) + (j * h / sh) * w];
                        if (SIO_IS_VIS (c))
                            sio_pixel_raw (x + i, y + j, c);
                    }
        }
}

void
sio_rectf_raw (int x, int y, int w, int h, uint8_t c)
{
    int i;
    int x0 = MAX (x, 0), x1 = MIN (x, 0);
    int y0 = MAX (y, 0), y1 = MIN (y, 0);
    for (i = 0; i < MIN (h + y1, s.canvas->height - y); ++i)
        memset (&s.canvas->data[x0 + (y0 + i) * s.canvas->width],
                c & s.recolor, MIN (w + x1, s.canvas->width - x));
}

void
sio_rectf (int x, int y, int w, int h, uint8_t c)
{
    if (x > -w && y > -h && x < s.canvas->width && y < s.canvas->height)
        sio_rectf_raw (x, y, w, h, c);
}

void
sio_rect (int x, int y, int w, int h, int t, uint8_t c)
{
    if (x > -w && y > -h && x < s.canvas->width && y < s.canvas->height)
        {
            sio_rectf_raw (x, y, w, t, c);
            sio_rectf_raw (x, y + h - t, w, t, c);
            sio_rectf_raw (x, y + t, t, h - 2 * t, c);
            sio_rectf_raw (x + w - t, y + t, t, h - 2 * t, c);
        }
}

#define LINE_TEMPLATE(DRAW)                                                   \
    int dx = ABS (ex - sx), xdir = sx < ex ? 1 : -1;                          \
    int dy = -ABS (ey - sy), ydir = sy < ey ? 1 : -1;                         \
    int e = dx + dy, e2;                                                      \
    for (;;)                                                                  \
        {                                                                     \
            DRAW;                                                             \
            if (sx == ex && sy == ey)                                         \
                break;                                                        \
            e2 = e * 2;                                                       \
            if (e2 >= dy)                                                     \
                e += dy, sx += xdir;                                          \
            if (e2 <= dx)                                                     \
                e += dx, sy += ydir;                                          \
        }

void
sio_line (int sx, int sy, int ex, int ey, uint8_t c)
{
    LINE_TEMPLATE (sio_pixel (sx, sy, c))
}

void
sio_linecircle (int sx, int sy, int ex, int ey, int r, uint8_t c)
{
    LINE_TEMPLATE (sio_circlef (sx, sy, r, c))
}

void
sio_linerect (int sx, int sy, int ex, int ey, int w, int h, uint8_t c)
{
    LINE_TEMPLATE (sio_rectf (sx - w / 2, sy - h / 2, w, h, c))
}
#undef LINE_TEMPLATE

void
sio_circle (int x, int y, int r, uint8_t c)
{
    int X = r, Y, e = 3 - 2 * r;
    for (Y = 0; Y < r / SQRT_2 + 1; ++Y)
        {
            sio_pixel (x + X, y + Y, c);
            sio_pixel (x + X, y - Y, c);
            sio_pixel (x - X, y + Y, c);
            sio_pixel (x - X, y - Y, c);

            sio_pixel (x + Y, y + X, c);
            sio_pixel (x + Y, y - X, c);
            sio_pixel (x - Y, y + X, c);
            sio_pixel (x - Y, y - X, c);

            if (e <= 0)
                e += (3 + 2 * Y) * 2;
            else
                {
                    e += (5 - 2 * X + 2 * Y) * 2;
                    X--;
                }
        }
}

void
sio_circlef (int x, int y, int r, uint8_t c)
{
    int X = r, Y, e = 3 - 2 * r;
    for (Y = 0; Y < r / SQRT_2 + 1; ++Y)
        {
            sio_rectf (x - X, y + Y, 2 * X, 1, c);
            sio_rectf (x - X, y - Y, 2 * X, 1, c);
            sio_rectf (x - Y, y + X, 2 * Y, 1, c);
            sio_rectf (x - Y, y - X, 2 * Y, 1, c);

            if (e <= 0)
                e += (3 + 2 * Y) * 2;
            else
                {
                    e += (5 - 2 * X + 2 * Y) * 2;
                    X--;
                }
        }
}
#endif

#ifdef SIO_SDL
#define SIO_EMU
#include <SDL2/SDL.h>
#include <stdio.h>

#define PRINT_SDL_ERROR(f)                                                    \
    fprintf (stderr, "[ERROR, SDL] " #f " failed: %s\n", SDL_GetError ())

#define PRINT_SDL_ERROR_EXTRA(f, fmt, ...)                                    \
    fprintf (stderr, "[ERROR, SDL] " #f " failed: %s," fmt "\n",              \
             SDL_GetError (), __VA_ARGS__)

SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *screen;
uint8_t *pixels;
float perf_freq;

int
emu_init ()
{
    if (SDL_Init (SDL_INIT_VIDEO))
        {
            PRINT_SDL_ERROR (SDL_Init);
            return -1;
        }

    window = SDL_CreateWindow ("sio", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, s.canvas->width,
                               s.canvas->height, 0);
    if (window == NULL)
        {
            PRINT_SDL_ERROR (SDL_CreateWindow);
            return -1;
        }
    SDL_ShowCursor (0);

    render = SDL_CreateRenderer (window, -1, 0);
    if (render == NULL)
        {
            PRINT_SDL_ERROR (SDL_CreateRenderer);
            return -1;
        }
    SDL_RenderSetLogicalSize (render, s.canvas->width, s.canvas->height);
    SDL_SetRenderDrawColor (render, 0, 0, 0, 0xff);
    SDL_RenderClear (render);
    SDL_RenderPresent (render);

    screen = SDL_CreateTexture (render, SDL_PIXELFORMAT_RGB332,
                                SDL_TEXTUREACCESS_STREAMING, s.canvas->width,
                                s.canvas->height);
    if (screen == NULL)
        {
            PRINT_SDL_ERROR (SDL_CreateTexture);
            return -1;
        }

    pixels = malloc (s.canvas->width * s.canvas->height);
    perf_freq = SDL_GetPerformanceFrequency ();

    return 0;
}

void
emu_deinit ()
{
    free (pixels);
    SDL_DestroyTexture (screen);
    SDL_DestroyRenderer (render);
    SDL_DestroyWindow (window);
    SDL_Quit ();
}

uint8_t
sio_rgb332 (uint8_t c)
{
    return (c & 0b11) | ((((c >> 2) & 0b11) * 7 / 3) << 2)
           | ((((c >> 4) & 0b11) * 7 / 3) << 5);
}

void
sio_remap ()
{
    int i;
    for (i = 0; i < s.screen.width * s.screen.height; ++i)
        pixels[i] = sio_rgb332 (s.screen.data[i]);
}

void
emu_loop ()
{
    static SDL_Event e;
    static uint8_t c;
    static uint_t end, oldend = 0;
    static float elapsed, delay = 16.666f;

    while (SDL_PollEvent (&e))
        {
            switch (e.type)
                {
                case SDL_QUIT:
                    s.go = false;
                    return;
                case SDL_KEYDOWN:
                    c = e.key.keysym.sym;
                    if (s.keydown != NULL)
                        s.keydown (c);
                    s.key[c] = true;
                    break;
                case SDL_KEYUP:
                    c = e.key.keysym.sym;
                    if (s.keyup != NULL)
                        s.keyup (c);
                    s.key[c] = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    c = e.button.button % 3;
                    if (s.buttondown != NULL)
                        s.buttondown (c);
                    s.mouse[0] = e.button.x, s.mouse[1] = e.button.y;
                    s.button[c] = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    c = e.button.button % 3;
                    if (s.buttonup != NULL)
                        s.buttonup (c);
                    s.mouse[0] = e.button.x, s.mouse[1] = e.button.y;
                    s.button[c] = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (s.mousemove != NULL)
                        s.mousemove (e.motion.x, e.motion.y);
                    s.mouse[0] = e.motion.x;
                    s.mouse[1] = e.motion.y;
                    break;
                }
        }

    sio_remap ();
    SDL_UpdateTexture (screen, NULL, pixels, s.canvas->width);
    SDL_RenderClear (render);
    SDL_RenderCopy (render, screen, NULL, NULL);
    SDL_RenderPresent (render);

    if (oldend == 0)
        {
            end = SDL_GetPerformanceCounter ();
            oldend = end;
        }
    else
        {
            oldend = end;
            end = SDL_GetPerformanceCounter ();
            elapsed = (end - oldend) / perf_freq * 1000.f - delay;
            delay = MAX (0, 16.666f - elapsed);
        }
    SDL_Delay (delay);
}

void
emu_stretch (int width, int height)
{
    SDL_SetWindowSize (window, width, height);
}
#endif

#ifndef SIO_EMU
int
emu_init ()
{
    return 0;
}

void
emu_deinit ()
{
}

void
emu_loop ()
{
}

void
emu_stretch (int width, int height)
{
}
#endif

#endif /* end of header guard */
