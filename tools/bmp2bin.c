// Converts a BMP to/from the strange format this game uses
//
// For backgrounds, they are in the format that the GBA uses for Mode 4,
// which is a simple 8bpp bitmap. It is a simple memcpy.
//
// For sprites and tiles, it is not converted to the GBA's tile format,
// rather it is also copied as-is and converted at runtime.
//
// The palette format consists of 256 3 byte colors, of r, g, b
// from 0-31. At runtime, they are packed into the 15-bit palette used
// by the GBA.
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

static FILE *open_file(const char *filename, const char *mode)
{
    FILE *f = fopen(filename, mode);
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    return f;
}

static uint32_t file_size(FILE *f)
{
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);
    return (uint32_t)len;
}

static void write_8(int c, FILE *f)
{
    if (fputc(c, f) == EOF)
    {
        printf("Error writing to file!\n");
        exit(1);
    }
}

static int read_8(FILE *f)
{
    int c = fgetc(f);
    if (c == EOF)
    {
        printf("Error reading from file!\n");
        exit(1);
    }
    return c;
}

static void write_bytes(const void *buf, size_t n, FILE *f)
{
    if (fwrite(buf, n, 1, f) != 1)
    {
        printf("Error writing to file!\n");
        exit(1);
    }
}

static void read_bytes(void *buf, size_t n, FILE *f)
{
    if (fread(buf, n, 1, f) != 1)
    {
        printf("Error reading from file!\n");
        exit(1);
    }
}

static void write_16(uint32_t value, FILE *f)
{
    write_8(value & 0xFF, f);
    write_8(value >> 8, f);
}

static void write_32(uint32_t value, FILE *f)
{
    write_16(value & 0xFFFF, f);
    write_16(value >> 16, f);
}
static int read_16(FILE *f)
{
    int x = read_8(f);
    int y = read_8(f);
    return x | (y << 8);
}

static int read_32(FILE *f)
{
    unsigned x = read_16(f);
    unsigned y = read_16(f);
    return x | (y << 16);
}
struct BMP_Header
{
    // Bitmap header
    char signature[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t bmp_offset;
    // BITMAPINFOHEADER
    uint32_t dib_size; // = 40
    int32_t width, height;
    uint16_t planes;
    uint16_t depth;
    uint32_t compression;
    uint32_t image_size;
    int32_t horiz_resolution;
    int32_t vert_resolution;
    uint32_t num_colors;
    uint32_t important_colors;
    uint8_t palette[256][4];
};
static const int BMP_HEADER_SIZE = 1078; // no sizeof allowed because padding

static void read_header(struct BMP_Header *header, FILE *f)
{
    read_bytes(header->signature, 2, f);
    header->file_size = read_32(f);
    header->reserved = read_32(f);
    header->bmp_offset = read_32(f);
    header->dib_size = read_32(f);
    header->width = read_32(f);
    header->height = read_32(f);
    header->planes = read_16(f);
    header->depth = read_16(f);
    header->compression = read_32(f);
    header->image_size = read_32(f);
    header->horiz_resolution = read_32(f);
    header->vert_resolution = read_32(f);
    header->num_colors = read_32(f);
    header->important_colors = read_32(f);
    read_bytes(header->palette, sizeof(header->palette), f);
}

static void write_header(const struct BMP_Header *header, FILE *f)
{
    write_bytes(header->signature, 2, f);
    write_32(header->file_size, f);
    write_32(header->reserved, f);
    write_32(header->bmp_offset, f);
    write_32(header->dib_size, f);
    write_32(header->width, f);
    write_32(header->height, f);
    write_16(header->planes, f);
    write_16(header->depth, f);
    write_32(header->compression, f);
    write_32(header->image_size, f);
    write_32(header->horiz_resolution, f);
    write_32(header->vert_resolution, f);
    write_32(header->num_colors, f);
    write_32(header->important_colors, f);
    write_bytes(header->palette, sizeof(header->palette), f);
}

static void convert_from_bmp(char *argv[])
{
    FILE *bmp = open_file(argv[1], "rb");
    FILE *out = open_file(argv[2], "wb");

    // check file extension
    const char *extension = strrchr(argv[2], '.');
    int do_palette = 0;

    if (extension != NULL && strcmp(extension, ".pal") == 0)
    {
        do_palette = 1;
    }

    uint32_t file_sz = file_size(bmp);
    struct BMP_Header header = {0};
    read_header(&header, bmp);
    int32_t bmp_size = abs(header.width) * abs(header.height);

    if (header.signature[0] != 'B' || header.signature[1] != 'M' || header.file_size != file_sz || header.bmp_offset > header.file_size || header.bmp_offset < BMP_HEADER_SIZE || (header.file_size - header.bmp_offset) > bmp_size || header.compression != 0 || header.depth != 8 || (header.num_colors != 0 && header.num_colors != 256) || header.dib_size != 40)
    {
        printf("Invalid or unsupported BMP!\n");
        exit(1);
    }
    // todo
    if (header.width < 0)
    {
        printf("I can only handle negative heights for now :(\n");
        exit(1);
    }
    // todo
    if (abs(header.width) % 4 != 0)
    {
        printf("BMP width is not a multiple of 4!\n");
        exit(1);
    }
    if (do_palette)
    {
        for (int i = 0; i < 256; i++)
        {
            write_8(header.palette[i][2] >> 3, out);
            write_8(header.palette[i][1] >> 3, out);
            write_8(header.palette[i][0] >> 3, out);
        }
        
    }
    else
    {
        fseek(bmp, header.bmp_offset, SEEK_SET);
        uint8_t *image_data = malloc(bmp_size); 
        if (image_data == NULL)
        {
            printf("Failed to allocate memory!\n");
            exit(1);
        }
        read_bytes(image_data, bmp_size, bmp);

        // bmps are upside down
        if (header.height < 0)
        {
            // negative height: don't flip
            write_bytes(image_data, bmp_size, out);
        }
        else
        {
            // positive height: flip it
            for (int32_t i = header.height; i-- > 0;)
            {
                write_bytes(image_data + i * header.width, header.width, out);
            }
        }
        free(image_data);
    }
    fclose(out);
    fclose(bmp);
}

static void convert_to_bmp(char *argv[])
{
    long width = strtol(argv[4], NULL, 0);
    long height = strtol(argv[5], NULL, 0);
    if (width <= 0 || height <= 0 || (width * height) % 4 != 0)
    {
        printf("Invalid width and/or height!\n");
        exit(1);
    }

    FILE *bin = open_file(argv[1], "rb");
    FILE *pal = open_file(argv[2], "rb");
    FILE *bmp = open_file(argv[3], "wb");

    size_t bin_size = file_size(bin);
    if (bin_size != width * height || bin_size % 4 != 0)
    {
        printf("Invalid size!\n");
        exit(1);
    }
    size_t file_sz = width * height + BMP_HEADER_SIZE;
    struct BMP_Header header = {
        .signature = {'B', 'M'},
        .file_size = file_sz,
        .reserved = 0,
        .bmp_offset = BMP_HEADER_SIZE,
        .dib_size = 40,
        .width = width,
        .height = height,
        .planes = 1,
        .depth = 8,
        .compression = 0,
        .image_size = 0,
        .horiz_resolution = 0,
        .vert_resolution = 0,
        .num_colors = 0,
        .important_colors = 0,
        .palette = {0}
    };

    // Read palette
    for (int i = 0; i < 256; i++)
    {
        // convert from 5 bit to 8 bit
        int r = read_8(pal) << 3;
        int g = read_8(pal) << 3;
        int b = read_8(pal) << 3;
        // convert to bgr
        header.palette[i][0] = b;
        header.palette[i][1] = g;
        header.palette[i][2] = r;
        header.palette[i][3] = 0;
    }
    // Convert image data
    uint8_t *image_data = calloc(1, bin_size);
    if (image_data == NULL)
    {
        printf("Failed to allocate memory!\n");
        exit(1);
    }

    // Bitmaps are upside down, flip it
    for (int32_t i = header.height; i-- > 0;)
    {
        read_bytes(image_data + i * header.width, header.width, bin);
    }
    write_header(&header, bmp);
    write_bytes(image_data, bin_size, bmp);
    free(image_data);
    fclose(bmp);
    fclose(pal);
    fclose(bin);
}

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        convert_from_bmp(argv);
    }
    else if (argc == 6)
    {
        convert_to_bmp(argv);
    }
    else
    {
        printf(
            "Usage:\n"
            "  %s in.bmp out.bin: Convert BMP to an image\n"
            "  %s in.bmp out.pal: Convert BMP to a palette\n"
            "  %s in.bin in.pal out.bmp width height: Convert back to BMP\n",
            argv[0], argv[0], argv[0]);
        return 1;
    }
}
