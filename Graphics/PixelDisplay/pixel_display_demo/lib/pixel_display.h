#pragma once

#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef _OS_OSX_
#include <OpenGL/gl.h>
#else
#include <gles2.h>
#include <gles2ext.h>
#endif

#include <ccr.h>

typedef void(*pixel_display_update_callback_t)(float timeStepInSeconds, int width, int height);
typedef void(*pixel_display_resize_callback_t)(int width, int height);

void pixel_display_init(int width, pixel_display_update_callback_t pixel_display_update_callback, pixel_display_resize_callback_t pixel_display_resize_callback);
void pixel_display_enter_loop();
void pixel_display_finish();
void pixel_display_set_pixel(int x, int y, int r255, int g255, int b255);
void pixel_display_clear_pixel();
