#include <stdlib.h>

#include "../../sio.h"
#include "sprites.h"

/* #define NUM (1 << 10) */
#define NUM (1 << 4)

int
main (int argc, char *argv[])
{
    int i;
    int pos[NUM][2];
    int8_t dir[NUM][2];
    uint8_t col[NUM];

    if (sio_init (360, 250))
        return -1;

    emu_stretch (s.screen.width * 2, s.screen.height * 2);

    s.bg = 0b010101;

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

            sio_blit (pig, s.screen.width / 2 - 20, s.screen.height / 2 - 20,
                      5, 5, 40, 40);

            sio_loop ();
        }

    sio_deinit ();
    return 0;
}
