/*
 * C implementations of drawing functions (and helper functions) 
 * CSF Assignment 2
 * Bob Dong / Aidan Alme
 * bdong9@jh.edu / aalme2@jhu.edu
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drawing_funcs.h"
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

/*
 * checks x and y coordinates to determine whether they are in bounds in the specified image.
 *
 * Parameters:
 * img - pointer to the image
 * x - x coordinate in need of checking
 * y - y cooridnate in need of checking
 *
 * Returns:
 * a boolean statement indicating whether x and y are in bounds or not.
 */

bool in_bounds(struct Image *img, int32_t x, int32_t y) {
  return (x < img->width) && (y < img->height);
}

/*
 * Checks if tile is within the specified image. 
 * 
 * Parameters:
 *   tilemap - pointer to the tilemap
 *   tile - pointer to the tile
 *
 * Returns:
 *   a boolean statement indicating whether x and y are in bounds or not. 
 */
bool check_valid_tile(struct Image *tilemap, const struct Rect *tile) {

  //loop throough x-axis
  for (int32_t i = 0; i < tile->width; i++) {
    //loop through y-axis
    for (int32_t j = 0; j < tile->height; j++) {
      if (!in_bounds(tilemap, tile->x + i, tile->y + j)) {
        return false;
      }
    }
  }
  return true;
}


/*                                                                                                  
 *  compute index of pixel in an "image's data array given its x and y coordinates.                 
 *                                                                                                   
 * Parameters:                                                                                       
 *   img - pointer to the image                                                                    
 *   x - x coordinate of the image
 *   y - y coordinate of the image
 *                                                                                                       
 * Returns:                                                                                              
 *   a uint32_t index containing the value of the index                                
 */
uint32_t compute_index(struct Image *img, int32_t x, int32_t y) {
  return x + img->width * y;
}

/*
 * Clamps a value between a minimum and maximum range.
 *
 * Parameters:
 * val - the value to be clamped
 * min - the minimum value of the range
 * max - the maximum value of the range
 *
 * Returns:
 * the clamped value, ensuring it is within the specified range
 */
int32_t clamp(int32_t val, int32_t min, int32_t max) {
  if (val <= min) return min;
  if (val >= max) return max;
  return val;
}

/*
 * Extracts the red component from a 32-bit color value.
 *
 * Parameters:
 * color - the 32-bit color value in ARGB format
 *
 * Returns:
 * the red component (8-bit) of the color
 */
uint8_t get_r(uint32_t color) {
  return color >> 24;
}

/*
 * Extracts the green component from a 32-bit color value.
 *
 * Parameters:
 * color - the 32-bit color value in ARGB format
 *
 * Returns:
 * the green component (8-bit) of the color
 */
uint8_t get_g(uint32_t color) {
  return (color << 8) >> 24;
}

/*
 * Extracts the blue component from a 32-bit color value.
 *
 * Parameters:
 * color - the 32-bit color value in ARGB format
 *
 * Returns:
 * the blue component (8-bit) of the color
 */
uint8_t get_b(uint32_t color) {
  return (color << 16) >> 24;
}

/*
 * Extracts the alpha component from a 32-bit color value.
 *
 * Parameters:
 * color - the 32-bit color value in ARGB format
 *
 * Returns:
 * the alpha component (8-bit) of the color
 */
uint8_t get_a(uint32_t color) {
  return (color << 24) >> 24;
}

/*
 * Blends the color components of the foreground and background colors based on the alpha value.
 *
 * Parameters:
 * fg - the foreground color in ARGB format
 * bg - the background color in ARGB format
 * alpha - the alpha value for blending
 *
 * Returns:
 * the blended color in ARGB format
 */
uint32_t blend_components(uint32_t fg, uint32_t bg, uint32_t alpha) {

  uint32_t red = clamp(((alpha * get_r(fg) + (255U - alpha) * get_r(bg)) / 255U), 0UL, 255U);
  uint32_t green = clamp(((alpha * get_g(fg) + (255U - alpha) * get_g(bg)) / 255U), 0UL, 255U);
  uint32_t blue = clamp(((alpha * get_b(fg) + (255U - alpha) * get_b(bg)) / 255U), 0UL, 255U);
  return (red << 24) + (green << 16) + (blue << 8) + 255U;
}

/*
 * Blends two colors based on the alpha value of the foreground color.
 *
 * Parameters:
 * fg - the foreground color in ARGB format
 * bg - the background color in ARGB format
 *
 * Returns:
 * the blended color in ARGB format
 */
uint32_t blend_colors(uint32_t fg, uint32_t bg) {
  return blend_components(fg, bg, get_a(fg));
}

/*
 * Sets a pixel in the image to a new color, blending it with the existing color.
 *
 * Parameters:
 * img - pointer to the image
 * index - the index of the pixel in the image data array
 * color - the new color to set the pixel to, in ARGB format
 */
void set_pixel(struct Image *img, uint32_t index, uint32_t color) {
  img->data[index] = blend_colors(color, img->data[index]);
}

/*
 * Computes the square of a number.
 *
 * Parameters:
 * x - the number to be squared
 *
 * Returns:
 * the square of the number
 */
int64_t square(int64_t x) {
  return x * x;
}

/*
 * Computes the squared distance between two points.
 *
 * Parameters:
 * x1, y1 - coordinates of the first point
 * x2, y2 - coordinates of the second point
 *
 * Returns:
 * the squared distance between the two points
 */
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
  return square(x1 - x2) + square(y1 - y2);
}

////////////////////////////////////////////////////////////////////////
// API functions
////////////////////////////////////////////////////////////////////////

//
// Draw a pixel.
//
// Parameters:
//   img   - pointer to struct Image
//   x     - x coordinate (pixel column)
//   y     - y coordinate (pixel row)
//   color - uint32_t color value
//
void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color) {
  if (in_bounds(img, x, y)) {
    set_pixel(img, compute_index(img, x, y), color);
  }
}

//
// Draw a rectangle.
// The rectangle has rect->x,rect->y as its upper left corner,
// is rect->width pixels wide, and rect->height pixels high.
//
// Parameters:
//   img     - pointer to struct Image
//   rect    - pointer to struct Rect
//   color   - uint32_t color value
//
void draw_rect(struct Image *img, const struct Rect *rect, uint32_t color) {
  for (int32_t i = 0; i < rect->width; i++) {
    for (int32_t j = 0; j < rect->height; j++) {
      draw_pixel(img, rect->x + i, rect->y + j, color);
    }
  }
}

//
// Draw a circle.
// The circle has x,y as its center and has r as its radius.
//
// Parameters:
//   img     - pointer to struct Image
//   x       - x coordinate of circle's center
//   y       - y coordinate of circle's center
//   r       - radius of circle
//   color   - uint32_t color value
//
void draw_circle(struct Image *img, int32_t x, int32_t y, int32_t r, uint32_t color) {
  for (int32_t i = -r; i <= r; i++) {
    for (int32_t j = -r; j <= r; j++) {
      if (square_dist(x, y, x + i, y + j) <= square(r)) {
	draw_pixel(img, x + i, y + j, color);
      }
    }
  }
}

//
// Draw a tile by copying all pixels in the region
// enclosed by the tile parameter in the tilemap image
// to the specified x/y coordinates of the destination image.
// No blending of the tile pixel colors with the background
// colors should be done.
//
// Parameters:
//   img     - pointer to Image (dest image)
//   x       - x coordinate of location where tile should be copied
//   y       - y coordinate of location where tile should be copied
//   tilemap - pointer to Image (the tilemap)
//   tile    - pointer to Rect (the tile)
//
void draw_tile(struct Image *img, int32_t x, int32_t y,  struct Image *tilemap, const struct Rect *tile) {
  if (!check_valid_tile(tilemap, tile)) {
    return;
  }
  for (int32_t i = 0; i < tile->width; i++) {
    for (int32_t j = 0; j < tile->height; j++) {
      if (in_bounds(img, x + i, y + j)) {
	img->data[compute_index(img, x + i, y + j)] = tilemap->data[compute_index(tilemap, tile->x + i, tile->y + j)];
      }
    }
  }
}

//
// Draw a sprite by copying all pixels in the region
// enclosed by the sprite parameter in the spritemap image
// to the specified x/y coordinates of the destination image.
// The alpha values of the sprite pixels should be used to
// blend the sprite pixel colors with the background
// pixel colors.
//
// Parameters:
//   img       - pointer to Image (dest image)
//   x         - x coordinate of location where sprite should be copied
//   y         - y coordinate of location where sprite should be copied
//   spritemap - pointer to Image (the spritemap)
//   sprite    - pointer to Rect (the sprite)
//
void draw_sprite(struct Image *img, int32_t x, int32_t y, struct Image *spritemap, const struct Rect *sprite) {
  if (!check_valid_tile(spritemap, sprite)) {
    return;
  }
  for (int32_t i = 0; i < sprite->width; i++) {
    for (int32_t j = 0; j < sprite->height; j++) {
      if (in_bounds(img, x + i, y + j)) {
        set_pixel(img, compute_index(img, x + i, y + j), spritemap->data[compute_index(spritemap, sprite->x + i, sprite->y + j)]);
      }
    }
  }
}
