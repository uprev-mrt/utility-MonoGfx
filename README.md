# MonoGfx

Module for monochromatic graphics buffering.

the mono_gfx_t struct can be initialized 'buffered' or 'unbuffered'. When it is buffered, it allocates its buffer in memory and works with the local copy. When it is unbuffered, all drawing functions are sent to the callback function for writing pixels. This allows the use of displays with areas too large to store in ram.



buffered example:
```
mono_gfx_t gfx;

//initialize a 128x32 canvase
mono_gfx_init_buffered(&gfx,128,32);

//draw a 30x20 rectangle at x,y = 5,5
mono_gfx_draw_rect(&gfx, 5,5,30,20);
```
