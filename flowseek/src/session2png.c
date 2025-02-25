#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <string.h>

#define IMAGE_SIZE 28
#define BYTES_PER_PIXEL 1

int save_as_png(const char *filename, const unsigned char *data) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Could not open %s for writing\n", filename);
        return -1;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return -1;
    }

    png_init_io(png, fp);

    // Set image properties
    png_set_IHDR(
        png,
        info,
        IMAGE_SIZE, IMAGE_SIZE,
        8, // Bit depth
        PNG_COLOR_TYPE_GRAY,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);

    // Write image data
    png_bytep row = (png_bytep) malloc(IMAGE_SIZE * BYTES_PER_PIXEL);
    for (int y = 0; y < IMAGE_SIZE; y++) {
        memcpy(row, &data[y * IMAGE_SIZE], IMAGE_SIZE);
        png_write_row(png, row);
    }
    free(row);

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(fp);

    return 0;
}

#if 0
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_png>\n", argv[0]);
        return 1;
    }

    // Read 784 bytes from input file
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("Failed to open input file");
        return 1;
    }

    unsigned char data[784];
    if (fread(data, 1, 784, fp) != 784) {
        fclose(fp);
        fprintf(stderr, "Input file must be exactly 784 bytes\n");
        return 1;
    }
    fclose(fp);

    // Save as PNG
    if (save_as_png(argv[2], data) != 0) {
        fprintf(stderr, "Failed to save PNG file\n");
        return 1;
    }

    printf("Successfully saved %s\n", argv[2]);
    return 0;
}
#endif
