/**
  *@file mono_gfx.h
  *@brief module for monochromatic graphics
  *@author Jason Berger
  *@date 03/15/2019
  */
#pragma once

#include "Platforms/Common/mrt_platform.h"

#define MONO_GFX_PIXEL_OFF 0
#define MONO_GFX_PIXEL_ON 1
#define MONO_GFX_PIXEL_INVERT 2

struct mono_gfx_struct;
typedef mrt_status_t (*f_mono_gfx_write_pixel)(struct mono_gfx_struct* gfx, int x, int y, uint8_t val);
typedef mrt_status_t (*f_mono_gfx_write)(struct mono_gfx_struct* gfx, int x, int y, uint8_t* data, int len, bool wrap); //pointer to write function
typedef mrt_status_t (*f_mono_gfx_read)(struct mono_gfx_struct* gfx, int x, int y, uint8_t* data, int len, bool wrap); //pointer to write function

typedef struct{
	const uint8_t* data;
	int width;
	int height;
}GFXBmp;

typedef struct { // Data stored PER GLYPH
	int bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;

typedef struct mono_gfx_struct{
  uint8_t* mBuffer;						 //buffer to store pixel data
  int mWidth;						 // width of buffer in pixels
  int mHeight;							//height of buffer in pixels
  uint32_t mBufferSize;					//size of buffer (in bytes)
	const GFXfont* mFont;       				//font to use for printing
  f_mono_gfx_write_pixel fWritePixel; //pointer to write function
	void* mDevice;								//void pointer to device for unbuffered implementation
	bool mBuffered;
} mono_gfx_t;

#ifdef __cplusplus
extern "C"
{
#endif


/**
  *@brief initializes a mono_gfx_t that manages its own buffer
  *@param gfx ptr to mono_gfx_t to be initialized
	*@param width width (in pixels) of display buffer
  *@param height height (in pixels) of display buffer
  *@return status
  */
mrt_status_t mono_gfx_init_buffered(mono_gfx_t* gfx, int width, int height);

/**
  *@brief initializes a mono_gfx_t that does not manage its own buffer. This is used for large displays where storing the buffer locally doesnt make sense
  *@param gfx ptr to mono_gfx_t to be initialized
	*@param width width (in pixels) of display buffer
  *@param height height (in pixels) of display buffer
	*@param write_cb call back to function to write pixels
	*@param dev void ptr to device (used as )
  *@return status
  */
mrt_status_t mono_gfx_init_unbuffered(mono_gfx_t* gfx, int width, int height, f_mono_gfx_write_pixel write_cb, void* dev );

/**
  *@brief frees deinitializes gfx object and frees buffer
  *@param gfx ptr to graphics object
  */
mrt_status_t mono_gfx_deinit(mono_gfx_t* gfx);

/**
  *@brief writes a single pixel on the canvas
  *@param gfx ptr to gfx object
	*@param x x coord to draw
  *@param y y coord to draw
	*@param val pixel value
  *@return status
  */
mrt_status_t mono_gfx_write_pixel(mono_gfx_t* gfx, int x, int y, uint8_t val);

/**
  *@brief writes an array of bytes to the buffer
  *@param gfx ptr to mono_gfx_t descriptor
  *@param data ptr to black data being written
  *@param len number of bytes being written
  *@param wrap whether or not to wrap when we reach the end of current row
  *@return status of operation
  */
mrt_status_t mono_gfx_write_buffer(mono_gfx_t* gfx, int x, int y, uint8_t* data, int len, bool wrap);

/**
  *@brief Draws a bitmap to the buffer
  *@param gfx ptr to mono_gfx_t descriptor
  *@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
  *@param bmp bitmap to draw
  *@param val pixel value on
  *@return status of operation
  */
mrt_status_t mono_gfx_draw_bmp(mono_gfx_t* gfx, int x, int y,const GFXBmp* bmp, uint8_t val);

/**
  *@brief Draws rendered text to the buffer
  *@param gfx ptr to mono_gfx_t descriptor
  *@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
  *@param text text to be written
  *@param val pixel value
  *@return status of operation
  */
mrt_status_t mono_gfx_print(mono_gfx_t* gfx, int x, int y, const char * text, uint8_t val);

/**
  *@brief draws a rectangle
  *@param gfx ptr to gfx canvas
	*@param x0 x coord of p1
  *@param y0 y coord of p1
	*@param x1 x coord of p2
  *@param y1 y coord of p2
  *@param val pixel value
  *@return "Return of the function"
  */
mrt_status_t mono_gfx_draw_line(mono_gfx_t* gfx, int x0, int y0, int x1, int y1, uint8_t val);

/**
  *@brief draws a rectangle
  *@param gfx ptr to gfx canvas
	*@param x x coord to begin drawing at
  *@param y y coord to begin drawing at
	*@param w width
  *@param h height
  *@param val pixel value
  *@return "Return of the function"
  */
mrt_status_t mono_gfx_draw_rect(mono_gfx_t* gfx, int x, int y, int w, int h, uint8_t val);

/**
  *@brief fill buffer with value
  *@param gfx ptr to gfxice
  *@param val value to write
  *@return status of operation
  */
mrt_status_t mono_gfx_fill(mono_gfx_t* gfx, uint8_t val);

#ifdef __cplusplus
}
#endif
