/**
  *@file FifoTest.cpp
  *@brief unit tests for the Fifo class
  *@author Jason Berger
  *@date 02/16/2019
  */
#ifdef UNIT_TESTING_ENABLED

#include "mono_gfx.c"
#include <gtest/gtest.h>



//8x8 pixels with line from top left to bottom right
const uint8_t compLine[] = {
  0x80, /* 1 _ _ _ _ _ _ _ */
  0x40, /* _ 1 _ _ _ _ _ _ */
  0x20, /* _ _ 1 _ _ _ _ _ */
  0x10, /* _ _ _ 1 _ _ _ _ */
  0x08, /* _ _ _ _ 1 _ _ _ */
  0x04, /* _ _ _ _ _ 1 _ _ */
  0x02, /* _ _ _ _ _ _ 1 _ */
  0x01  /* _ _ _ _ _ _ _ 1 */
};

//8x8 pixels with inner 6x6 rect filled in
const uint8_t compRect[] = {
  0x00, /* _ _ _ _ _ _ _ _ */
  0x7E, /* _ 1 1 1 1 1 1 _ */
  0x7E, /* _ 1 1 1 1 1 1 _ */
  0x7E, /* _ 1 1 1 1 1 1 _ */
  0x7E, /* _ 1 1 1 1 1 1 _ */
  0x7E, /* _ 1 1 1 1 1 1 _ */
  0x7E, /* _ 1 1 1 1 1 1 _ */
  0x00 /* _ _ _ _ _ _ _ _ */
};


mono_gfx_t canvas;

//Test drawing a rectangle
TEST(MonoGfxTest, rectTest)
{
    mono_gfx_init_buffered(&canvas, 8,8);

    mono_gfx_draw_rect(&canvas, 1,1,6,6, 1);

    for(int i=0; i < canvas.mBufferSize; i++)
    {
      ASSERT_EQ(compRect[i], canvas.mBuffer[i]) << "failed at index:" << i ;
    }

    mono_gfx_deinit(&canvas);
}

//Test drawing a rectangle
TEST(MonoGfxTest, lineTest)
{
    mono_gfx_init_buffered(&canvas, 8,8);

    mono_gfx_draw_line(&canvas, 0,0,7,7,1);

    for(int i=0; i < canvas.mBufferSize; i++)
    {
      ASSERT_EQ(compLine[i], canvas.mBuffer[i]) << "failed at index:" << i ;
    }

    mono_gfx_deinit(&canvas);
}

#endif
