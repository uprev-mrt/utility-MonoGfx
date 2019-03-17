/**
  *@file mono_gfx.c
  *@brief module for monochromatic graphics
  *@author Jason Berger
  *@date 03/15/2019
  */

#include "mono_gfx.h"
#include "string.h"
#include <stdlib.h>


mrt_status_t mono_gfx_init_buffered(mono_gfx_t* gfx, int width, int height)
{
  gfx->mBufferSize = (width * height)/8;
  gfx->mBuffer = (uint8_t*) malloc(gfx->mBufferSize);
  memset(gfx->mBuffer,0,gfx->mBufferSize);
  gfx->mWidth = width;
  gfx->mHeight = height;
  gfx->mFont  = NULL;
  gfx->fWritePixels = &mono_gfx_write_buffer;
  gfx->mDevice  = NULL;
  gfx->mBuffered = true;

  return MRT_STATUS_OK;
}


mrt_status_t mono_gfx_init_unbuffered(mono_gfx_t* gfx, int width, int height, mono_gfx_write write_cb, void* dev )
{
  gfx->mBufferSize = (width * height)/8;
  gfx->mBuffer = NULL;
  gfx->mWidth = width;
  gfx->mHeight = height;
  gfx->mFont  = NULL;
  gfx->fWritePixels = &write_cb;
  gfx->mDevice  = dev;
  gfx->mBuffered = false;

  return MRT_STATUS_OK;
}

mrt_status_t mono_gfx_write_pixel(mono_gfx_t* gfx, int x, int y, uint8_t val)
{
  if(( x < 0) || (x >= gfx->mWidth) || (y < 0) || (y>= gfx->mHeight))
    return MRT_STATUS_OK;

    uint32_t cursor = (y * gfx->mWidth) + x;
    uint8_t mask = 0x80;

    //get number of bits off of alignment in case we are not writing on a byte boundary
    uint32_t byteOffset = (cursor  / 8);
    uint8_t bitOffset = cursor % 8;
    mask = mask >> bitOffset;

    if( val == 0)
      gfx->mBuffer[byteOffset] &= (~mask);
    else
      gfx->mBuffer[byteOffset] |= mask;

    return MRT_STATUS_OK;

}


mrt_status_t mono_gfx_write_buffer(mono_gfx_t* gfx, int x, int y, uint8_t* data, int len, bool wrap)
{
  uint32_t cursor = (y * gfx->mWidth) + x;

  //get number of bits off of alignment in case we are not writing on a byte boundary
  uint32_t byteOffset = (cursor  / 8);
  uint8_t bitOffset = cursor % 8;

  //get number of bytes before we would wrap to next row
  int nextRow = (gfx->mWidth - (cursor % gfx->mWidth));
  if((nextRow < len) && (wrap == false))
  {
    len = nextRow;
  }


  uint8_t prevByte; //used for shifting in data when not aligned
  uint8_t mask;


  //If we are byte aligned , just memcpy the data in
  if(bitOffset == 0)
  {
    memcpy(&gfx->mBuffer[byteOffset], data, len);
  }
  //If we are not byte aligned, we have to mask and shift in data
  else
  {
    mask = 0xFF << (8-bitOffset);
    prevByte = gfx->mBuffer[byteOffset] & mask;

    for(int i=0; i < len; i++)
    {
      gfx->mBuffer[byteOffset++] = prevByte | (data[i] >> bitOffset);
      prevByte = data[i] << (8-bitOffset);

      if(byteOffset >= gfx->mBufferSize)
        byteOffset = 0;
    }
  }


  //advance cursor
  cursor += len;

  // If its gone over, wrap
  while(cursor >= (gfx->mWidth * gfx->mHeight))
    cursor -=  (gfx->mWidth * gfx->mHeight);

  return MRT_STATUS_OK;
}


mrt_status_t mono_gfx_draw_bmp(mono_gfx_t* gfx, int x, int y, GFXBmp* bmp)
{
  uint32_t bmpIdx = 0;

  uint8_t mask =0x80;
  int bit =0;
  int i,a;

  for(i=0; i < bmp->height; i ++)
  {
    for(a=0; a < bmp->width; a++)
    {
      bit = a %8;
      mono_gfx_write_pixel(gfx, x+a, y+i, ((bmp->data[bmpIdx/8] << bit) & mask));
      bmpIdx ++;
    }

  }
  return MRT_STATUS_OK;
}


mrt_status_t mono_gfx_print(mono_gfx_t* gfx, int x, int y, const char * text)
{

  //if a font has not been set, return error
  if(gfx->mFont == NULL)
    return MRT_STATUS_ERROR;

  int xx =x;     //current position for writing
  int yy = y;
  GFXglyph* glyph;    //pointer to glyph for current character
  GFXBmp bmp;         //bitmap struct used to draw glyph
  char c = *text++;   //grab first character from string

  //run until we hit a null character (end of string)
  while(c != 0)
  {
    if(c == '\n')
    {
      //if character is newline, we advance the y, and reset x
      yy+= gfx->mFont->yAdvance;
      xx = x;
    }
    else if((c >= gfx->mFont->first) && (c <= gfx->mFont->last))// make sure the font contains this character
    {
      //grab the glyph for current character from our font
      glyph = &gfx->mFont->glyph[c - gfx->mFont->first]; //index in glyph array is offset by first printable char in font

      //map glyph to a bitmap that we can draw
      bmp.data = &gfx->mFont->bitmap[glyph->bitmapOffset];
      bmp.width = glyph->width ;
      bmp.height = glyph->height ;

      //draw the character
      mono_gfx_draw_bmp(gfx, xx + glyph->xOffset, yy + glyph->yOffset, &bmp );
      xx += glyph->xOffset + glyph->xAdvance;
    }


    //get next character
    c = *text++;
  }

  return MRT_STATUS_OK;
}

mrt_status_t mono_gfx_draw_rect(mono_gfx_t* gfx, int x, int y, int w, int h)
{
  for(int i=0; i < h; i++)
  {
    for(int a=0; a < w; a++)
    {
      mono_gfx_write_pixel(gfx,x+a, y+i, 1);
    }
  }

  return MRT_STATUS_OK;
}


mrt_status_t mono_gfx_fill(mono_gfx_t* gfx, uint8_t val)
{
  if(gfx->mBuffered)
  {
    memset(gfx->mBuffer, val, gfx->mBufferSize);
  }
  else
  {
    //TODO
  }
  return MRT_STATUS_OK;
}
