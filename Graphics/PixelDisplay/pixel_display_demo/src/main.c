#include "../lib/pixel_display.h"

float dot_x = 0, dot_y = 0;
float dot_vx = 0, dot_vy = 0;

void update_dot(float timeStepInSeconds, int width, int height)
{
	dot_x += dot_vx * timeStepInSeconds;
	dot_y += dot_vy * timeStepInSeconds;
	if (dot_x < 0)
	{
		dot_vx = -dot_vx;
		dot_x = 0;
	}
	if (dot_x >= width)
	{
		dot_vx = -dot_vx;
		dot_x = width - 1;
	}
	if (dot_y < 0)
	{
		dot_vy = -dot_vy;
		dot_y = 0;
	}
	if (dot_y >= height)
	{
		dot_vy = -dot_vy;
		dot_y = height - 1;
	}
}

void pixel_display_update_callback(float timeStepInSeconds, int width, int height)
{
	update_dot(timeStepInSeconds, width, height);

	pixel_display_clear_pixel();

	pixel_display_set_pixel(dot_x, dot_y, 255, 0, 0);
}

void pixel_display_resize_callback(int width, int height)
{
	dot_x = width / 2;
	dot_y = height / 2;

	dot_vx = 10;
	dot_vy = 20;
}

int main()
{
	pixel_display_init(40, pixel_display_update_callback, pixel_display_resize_callback);

	pixel_display_enter_loop();

	pixel_display_finish();

	return 0;
}
