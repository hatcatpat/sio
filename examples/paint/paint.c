#include "../../sio.h"

int
main (int argc, char *argv[])
{
    int oldmouse[2] = { 0, 0 };
    int r = 4;
    bool held = false, circle = false;
    uint8_t color = SIO_RED;
    sio_canvas_t canvas;

    if (sio_init (360, 250))
        return -1;

    s.bg = SIO_BLACK;
    sio_canvas_init (&canvas, s.screen.width, s.screen.height);

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

            sio_rectf (4, 4, 4, 4, color);

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
    sio_deinit ();
    return 0;
}
