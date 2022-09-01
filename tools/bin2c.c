#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static void bin2c_u8(FILE *in, FILE *out)
{
    size_t i = 0;
    for (int c = fgetc(in); c != EOF; i++)
    {
        fprintf(out, "0x%02x", c);
        if ((c = fgetc(in)) != EOF) {
            fputc(',', out);
        } else {
            break;
        }
        
        if (i % 32 == 31) fprintf(out, "\n    ");
        else fprintf(out, " ");
    }
    fprintf(out, "\n};\n");

}
static void bin2c_u32(FILE *in, FILE *out)
{
    unsigned char buf[4] = { 0 };
    size_t i = 0;
    size_t count ;
    while ((count = fread(buf, 1, 4, in)) == 4)
    {
        uint32_t value = buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
        fprintf(out, "0x%08x", value);
        int c;
        if ((c = fgetc(in)) != EOF) {
             fputc(',', out);
            ungetc(c, in);
        } else {
            break;
        }
        if (i++ % 8 == 7) {
            fprintf(out, "\n    ");
        } else {
            fprintf(out, " ");
        }
        memset(buf, 0, 4);
    }
    fprintf(out, "\n};\n");
    if (count != 0 && count != 4)
    {
        printf("WARNING: Uneven byte count!\n");
    }
}
int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("usage: %s inputfile out.h [8|32]\n", argv[0]);
        return 1;
    }
    int size = 0;
    if (sscanf(argv[3], "%i", &size) != 1 || (size != 8 && size != 32)) {
        printf("invalid size, choose 8, 16, or 32\n");
        return 1;
    }
   FILE *in = fopen(argv[1], "rb");
    if (!in) {
        printf("cannot open %s\n", argv[1]);
        return 1;
    }
    FILE *out = fopen(argv[2], "wb");
    if (!out) {
        fclose(in);
        printf("cannot open %s\n", argv[2]);
        return 1;
    }
    char *dot = strrchr(argv[1], '.');
    if (dot) *dot = '\0';
    char *end = strrchr(argv[1], '/');
    if (!end++) end = argv[1];
    fprintf(out, "const u%d %s[] = {\n    ", size, end);
    if (size == 8) bin2c_u8(in, out);
    else bin2c_u32(in, out);

    fclose(in);
}