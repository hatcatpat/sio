#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../sio.h"
#include "sprites.h"

void
bulk ()
{
    int i;
#define NUM (1 << 10)
    int pos[NUM][2];
    int8_t dir[NUM][2];
    uint8_t col[NUM];
    for (i = 0; i < NUM; ++i)
        {
            pos[i][0] = rand () % (s.screen.width - 4);
            pos[i][1] = rand () % (s.screen.height - 4);
            dir[i][0] = (rand () % 2) == 0 ? -1 : 1;
            dir[i][1] = (rand () % 2) == 0 ? -1 : 1;
            col[i] = rand () % SIO_WHITE;
        }

    while (s.go)
        {
            if (s.key['q'])
                break;

            sio_clear ();

            for (i = 0; i < NUM; ++i)
                {
                    if (pos[i][0] < 0)
                        dir[i][0] = 1;
                    else if (pos[i][0] >= s.screen.width - 4)
                        dir[i][0] = -1;
                    pos[i][0] += dir[i][0];
                    if (pos[i][1] < 0)
                        dir[i][1] = 1;
                    else if (pos[i][1] >= s.screen.height - 4)
                        dir[i][1] = -1;
                    pos[i][1] += dir[i][1];
                    s.recolor = col[i];
                    sio_bliti (pig, pos[i][0], pos[i][1], 5, 5);
                }
            s.recolor = SIO_WHITE;

            sio_loop ();
        }
}

void
paint ()
{
    s.bg = SIO_BLACK;

    sio_canvas_t canvas;
    sio_canvas_init (&canvas, s.screen.width, s.screen.height);

    int oldmouse[2] = { 0, 0 };
    int r = 4;
    bool held = false;
    uint8_t color = SIO_RED;
    bool circle = false;

    while (s.go)
        {
            if (s.key['q'])
                break;

            if (s.key['a'])
                r = MAX (1, r - 1);
            else if (s.key['d'])
                r = MIN (128, r + 1);

            if (sio_pressed ('1'))
                color = SIO_RED;
            else if (sio_pressed ('2'))
                color = SIO_BLUE;
            else if (sio_pressed ('3'))
                color = SIO_GREEN;

            if (sio_pressed ('z'))
                circle = false;
            else if (sio_pressed ('x'))
                circle = true;

            sio_bliti (canvas.data, 0, 0, s.screen.width, s.screen.height);

            sio_rect (40, 40, 50, 20, 4, color);

            if (s.button[0] || s.button[1])
                {
                    s.canvas = &canvas;
                    {
                        if (!held)
                            held = true;
                        else if (circle)
                            sio_linecircle (oldmouse[0], oldmouse[1] - r,
                                            s.mouse[0], s.mouse[1] - r, r,
                                            s.button[1] ? color : s.bg);
                        else
                            sio_linerect (oldmouse[0], oldmouse[1] - r,
                                          s.mouse[0], s.mouse[1] - r, 2 * r,
                                          2 * r, s.button[1] ? color : s.bg);
                        oldmouse[0] = s.mouse[0], oldmouse[1] = s.mouse[1];
                    }
                    s.canvas = &s.screen;
                }
            else if (held)
                held = false;

            if (circle)
                sio_circle (
                    s.mouse[0], s.mouse[1] - r, r,
                    ~canvas.data[s.mouse[0] + (s.mouse[1]) * canvas.width]);
            else
                sio_rect (
                    s.mouse[0] - r, s.mouse[1] - r - r, r * 2, r * 2, 1,
                    ~canvas.data[s.mouse[0] + (s.mouse[1]) * canvas.width]);

            sio_loop ();
        }

    sio_canvas_deinit (&canvas);
}

int
main (int argc, char *argv[])
{
    if (sio_init (360, 250))
        return -1;

    bulk ();

    sio_deinit ();
    return 0;
}
