#include "../../sio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNDO_STACK_MAX 16

typedef struct
{
    int x, y, width, height;
    uint8_t *data;
} undorect_t;
undorect_t undostack[UNDO_STACK_MAX] = { 0 };
uint8_t undostackp = 0;
undorect_t *undostacktop = NULL;
undorect_t *undo_push ();
undorect_t *undo_pop ();
void undo ();
void undorect_update (undorect_t *r, int x, int y);
void undorect_snapshot (undorect_t *r);

undorect_t *
undo_push ()
{
    undorect_t *r = &undostack[undostackp++];
    if (undostackp == UNDO_STACK_MAX)
        undostackp = 0;
    if (r->data != NULL)
        free (r->data);
    memset (r, 0, sizeof (undorect_t));
    r->x = r->y = -1;
    return r;
}

undorect_t *
undo_pop ()
{
    uint8_t p = undostackp == 0 ? UNDO_STACK_MAX - 1 : undostackp - 1;
    if (undostack[p].data != NULL)
        undostackp = p;
    return &undostack[p];
}

void
undo ()
{
    undorect_t *r = undo_pop ();
    if (r != NULL && r->data != NULL)
        {
            int i;
            for (i = 0; i < r->height; ++i)
                memcpy (&s.canvas->data[r->x + (r->y + i) * s.canvas->width],
                        &r->data[i * r->width], r->width);
            free (r->data);
            r->data = NULL;
        }
}

void
undorect_update (undorect_t *r, int x, int y)
{
    if (r->x == -1)
        r->x = MAX (x, 0);
    else if (x < r->x)
        {
            x = MAX (x, 0);
            r->width += r->x - x;
            r->x = x;
        }
    else if (x > r->x + r->width)
        r->width = x - r->x;

    if (r->y == -1)
        r->y = MAX (y, 0);
    if (y < r->y)
        {
            y = MAX (y, 0);
            r->height += r->y - y;
            r->y = y;
        }
    else if (y > r->y + r->height)
        r->height = y - r->y;
}

void
undorect_snapshot (undorect_t *r)
{
    int i;
    r->data = malloc (r->width * r->height);
    for (i = 0; i < r->height; ++i)
        memcpy (&r->data[i * r->width],
                &s.canvas->data[r->x + (r->y + i) * s.canvas->width],
                r->width);
}

int
main (int argc, char *argv[])
{
    int oldmouse[2] = { 0, 0 };
    int r = 4;
    bool held = false, circle = false;
    uint8_t fg = SIO_RED, bg = SIO_BLACK;
    sio_canvas_t canvas, oldcanvas;

    if (sio_init (360, 250))
        return -1;

    sio_canvas_init (&canvas, s.screen.width, s.screen.height);
    sio_canvas_init (&oldcanvas, s.screen.width, s.screen.height);

    sio_target (&oldcanvas);
    sio_clear (bg);
    sio_target (NULL);

    while (s.go)
        {
            if (s.key['q'])
                break;

            if (s.key['a'])
                r = MAX (1, r - 1);
            else if (s.key['d'])
                r = MIN (128, r + (s.mods[2] ? 10 : 1));

            if (sio_pressed ('1'))
                fg = SIO_RED;
            else if (sio_pressed ('2'))
                fg = SIO_BLUE;
            else if (sio_pressed ('3'))
                fg = SIO_GREEN;

            if (sio_pressed ('l'))
                sio_canvas_load (&oldcanvas, "saved.sio");
            else if (sio_pressed ('s'))
                sio_canvas_save (&oldcanvas, "saved.sio");

            if (sio_pressed ('z'))
                circle = false;
            else if (sio_pressed ('x'))
                circle = true;

            if (sio_pressed ('c'))
                {
                    undostacktop = undo_push ();
                    undostacktop->x = undostacktop->y = 0;
                    undostacktop->width = s.screen.width;
                    undostacktop->height = s.screen.height;

                    sio_target (&oldcanvas);
                    undorect_snapshot (undostacktop);
                    sio_clear (bg);
                    sio_target (NULL);
                }

            if (sio_pressed ('u'))
                {
                    sio_target (&oldcanvas);
                    undo ();
                    sio_target (NULL);
                }

            sio_bliti (oldcanvas.data, 0, 0, s.screen.width, s.screen.height);
            sio_bliti (canvas.data, 0, 0, s.screen.width, s.screen.height);
            sio_rectf (4, 4, 4, 4, fg);

            if (s.button[0] || s.button[1])
                {
                    sio_target (&canvas);

                    if (!held)
                        {
                            undostacktop = undo_push ();
                            held = true;
                        }
                    else if (circle)
                        sio_linecircle (oldmouse[0], oldmouse[1] - r,
                                        s.mouse[0], s.mouse[1] - r, r,
                                        s.button[1] ? fg : bg);
                    else
                        sio_linerect (oldmouse[0], oldmouse[1] - r, s.mouse[0],
                                      s.mouse[1] - r, 2 * r, 2 * r,
                                      s.button[1] ? fg : bg);

                    oldmouse[0] = s.mouse[0], oldmouse[1] = s.mouse[1];

                    undorect_update (undostacktop, s.mouse[0] - r * 2,
                                     s.mouse[1] - r * 2);
                    undorect_update (undostacktop, s.mouse[0] + r * 2,
                                     s.mouse[1] + r * 2);

                    sio_target (NULL);
                }
            else if (held)
                {
                    sio_target (&oldcanvas);
                    undorect_snapshot (undostacktop);
                    sio_bliti (canvas.data, 0, 0, s.screen.width,
                               s.screen.height);
                    sio_target (NULL);
                    memset (canvas.data, SIO_INVIS,
                            s.screen.width * s.screen.height);
                    held = false;
                }

            if (circle)
                sio_circle (s.mouse[0], s.mouse[1] - r, r,
                            ~oldcanvas.data[s.mouse[0]
                                            + s.mouse[1] * oldcanvas.width]);
            else
                sio_rect (s.mouse[0] - r, s.mouse[1] - r - r, r * 2, r * 2, 1,
                          ~oldcanvas.data[s.mouse[0]
                                          + s.mouse[1] * oldcanvas.width]);

            sio_loop ();
        }

    sio_canvas_deinit (&oldcanvas);
    sio_canvas_deinit (&canvas);
    sio_deinit ();
    return 0;
}
