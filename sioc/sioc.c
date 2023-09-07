#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int int16_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long int int64_t;
typedef unsigned long int uint64_t;
typedef unsigned int uint_t;

/* #define DEBUG */

uint8_t invisible = 0b1000000;
char *name = NULL;
char headerfmt[] = "uint8_t %s[%i * %i] = {";
char datafmt[] = "0x%x,";
char footer[] = "};";

uint8_t
rgb2sio (uint32_t rgb)
{
    uint8_t s = 0b11 * (rgb & 0xff) / 0xff;
    s |= (0b11 * ((rgb >> 8) & 0xff) / 0xff) << 2;
    s |= (0b11 * ((rgb >> 16) & 0xff) / 0xff) << 4;
    return s;
}

void
tga2sio (FILE *f)
{
    struct
    {
        uint8_t id_len;
        uint8_t cm_type;
        uint8_t img_type;
        uint16_t cm_first;
        uint16_t cm_len;
        uint8_t cm_bits_per_entry;
        uint16_t x_origin;
        uint16_t y_origin;
        uint16_t width;
        uint16_t height;
        uint8_t bits_per_pixel;
        uint8_t img_desc;
    } tga;
    int i;

    fread (&tga.id_len, 1, 1, f);
    fread (&tga.cm_type, 1, 1, f);
    fread (&tga.img_type, 1, 1, f);
    fread (&tga.cm_first, 2, 1, f);
    fread (&tga.cm_len, 2, 1, f);
    fread (&tga.cm_bits_per_entry, 1, 1, f);
    fread (&tga.x_origin, 2, 1, f);
    fread (&tga.y_origin, 2, 1, f);
    fread (&tga.width, 2, 1, f);
    fread (&tga.height, 2, 1, f);
    fread (&tga.bits_per_pixel, 1, 1, f);
    fread (&tga.img_desc, 1, 1, f);

#ifdef DEBUG
    printf ("tga info\n");
    printf ("id_len %i\n", tga.id_len);
    printf ("cm_type %i\n", tga.cm_type);
    printf ("img_type %i\n", tga.img_type);
    printf ("cm_first %i\n", tga.cm_first);
    printf ("cm_len %i\n", tga.cm_len);
    printf ("cm_bits_per_entry %i\n", tga.cm_bits_per_entry);
    printf ("x_origin %i\n", tga.x_origin);
    printf ("y_origin %i\n", tga.y_origin);
    printf ("width %i\n", tga.width);
    printf ("height %i\n", tga.height);
    printf ("bits_per_pixel %i\n", tga.bits_per_pixel);
    printf ("img_desc %i\n", tga.img_desc);
#endif

    if (tga.id_len != 0)
        fseek (f, tga.id_len, SEEK_CUR);

    printf (headerfmt, name, tga.width, tga.height);

    switch (tga.img_type)
        {
        case 1:
            {
                /* NOTE: assumed 24 bits_per_entry and 8 bits_per_pixel */
                uint32_t *cm = malloc (tga.cm_len * 4);
                uint8_t *data = malloc (tga.width * tga.height);
                int c;
                for (i = 0; i < tga.cm_len; ++i)
                    fread (&cm[i], 3, 1, f);
                for (i = 0; i < tga.width * tga.height; ++i)
                    data[i] = (c = fgetc (f)) ? rgb2sio (cm[c]) : invisible;
                free (cm);
                /* TODO: check img_desc for pixel direction */
                for (i = tga.width - 1; i >= 0; --i)
                    for (c = 0; c < tga.height; ++c)
                        printf (datafmt, data[c + i * tga.height]);
                free (data);
            }
            break;

        case 2:
            /* NOTE: assumed 24 bits_per_pixel (rgb) */
            {
                uint32_t c = 0;
                for (i = 0; i < tga.width * tga.height; ++i)
                    {
                        fread (&c, tga.bits_per_pixel / 8, 1, f);
                        printf (datafmt, rgb2sio (c));
                    }
            }
            break;

        case 3:
            /* NOTE: assumed 8 bits_per_pixel */
            for (i = 0; i < tga.width * tga.height; ++i)
                printf (datafmt, rgb2sio (fgetc (f)));
            break;
        }
    puts (footer);
}

int
main (int argc, char *argv[])
{
    while (argc-- > 1)
        {
            char *filename = *(argv++ + 1);
            FILE *f = fopen (filename, "rb");
            if (f == NULL)
                {
                    fprintf (stderr, "[error] unable to open %s\n", filename);
                    return -1;
                }
            name = strrchr (filename, '.');
            if (name != NULL)
                *name = '\0';
            name = strrchr (filename, '/');
            if (name == NULL)
                name = filename;
            else
                name++;
            tga2sio (f);
            fclose (f);
        }
    return 0;
}
