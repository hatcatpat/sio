#include "../sio.h"
#include "sprites.h"

int
main (int argc, char *argv[])
{
    if (sio_init (500, 500))
        return -1;

    s.bg = 0b010101;

    while (s.go)
        {
            if (s.key['q'])
                break;

            sio_clear ();
            sio_bliti (mountain, 10, 10, 400, 266);
            sio_blit (test, 10, 150 + 10, 5, 5, 40, 40);
            sio_loop ();
        }

    sio_deinit ();
    return 0;
}
