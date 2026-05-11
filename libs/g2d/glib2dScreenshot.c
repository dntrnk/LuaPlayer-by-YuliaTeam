#include "glib2dScreenshot.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

int g2dScreenshot(const char *path, int width, int height) {
  FILE *file = fopen(path, "wb");
  if (!file)
    return G2D_ERR_OPEN;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    return G2D_ERR_WRITESTRUCT;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return G2D_ERR_INFOSTRUCT;
  }

  png_init_io(png_ptr, file);
  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);

  uint8_t *line = (uint8_t *)malloc(width * 3);
  if (!line) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(file);
    return G2D_ERR_MEMALLOC;
  }

  int x, y;
  for (y = 0; y < height; y++) {
    int orig_y = (y * g2d_disp_buffer.h) / height;
    g2dColor *row = &g2d_disp_buffer.data[orig_y * 512];

    for (x = 0; x < width; x++) {
      int orig_x = (x * g2d_disp_buffer.w) / width;

      uint8_t r = G2D_GET_R(row[orig_x]);
      uint8_t g = G2D_GET_G(row[orig_x]);
      uint8_t b = G2D_GET_B(row[orig_x]);

      line[x * 3] = r;
      line[x * 3 + 1] = g;
      line[x * 3 + 2] = b;
    }
    png_write_row(png_ptr, line);
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  free(line);
  fclose(file);

  return 0;
}