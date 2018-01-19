#if !defined(_OS_OSX_) && !defined(_OS_WINDOWS_)
#error "Not supported platform!"
#endif

#include <stdio.h>
#include <stdbool.h>

#if defined(_OS_OSX_)
#include <OpenGL/gl.h>
#elif defined(_OS_WINDOWS_)
#include <GL/glew.h>
#endif

#include <ccr.h>

bool increaseGreen = true;
double green = 0.0;

static void drawAnObject()
{
	glColor3f(1.0f, green, 0.35f);

	green += increaseGreen ? 0.01f : -0.01f;
	if (green > 1.0)
	{
		green = 1.0;
		increaseGreen = false;
	}
	else if (green < 0.0)
	{
		green = 0;
		increaseGreen = true;
	}

	glBegin(GL_TRIANGLES);
	{
		glVertex3f(0.0, 0.6, 0.0);
		glVertex3f(-0.2, -0.3, 0.0);
		glVertex3f(0.2, -0.3, 0.0);
	}
	glEnd();
}

//OpenGL handlers : init , final , update , render , input
void on_OpenGL_Init(int view_w, int view_h)
{
}

void on_OpenGL_Final()
{
}

void on_OpenGL_Size(int view_w, int view_h)
{
	printf("on_OpenGL_Size(%d,%d);\n", view_w, view_h);
	glViewport(0, 0, view_w, view_h);
}

void on_OpenGL_Update(float timeStepInSeconds)
{
}

void on_OpenGL_Render()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	drawAnObject();

	glFlush();
}

void on_OpenGL_TouchBegin(float x, float y)
{
	printf("on_OpenGL_TouchBegin(x=%f,y=%f);\n", x, y);
}

void on_OpenGL_TouchMove(float prev_x, float prev_y, float x, float y)
{
	printf("on_OpenGL_TouchMove(prev_x=%f,prev_y=%f,x=%f,y=%f);\n",
		prev_x, prev_y, x, y);
}

void on_OpenGL_TouchEnd(float x, float y)
{
	printf("on_OpenGL_TouchEnd(x=%f,y=%f);\n", x, y);
}

int main()
{
	ccrSet_OpenGL_Init_Callback(on_OpenGL_Init);
	ccrSet_OpenGL_Final_Callback(on_OpenGL_Final);
	ccrSet_OpenGL_Size_Callback(on_OpenGL_Size);
	ccrSet_OpenGL_Update_Callback(on_OpenGL_Update);
	ccrSet_OpenGL_Render_Callback(on_OpenGL_Render);
	ccrSet_OpenGL_TouchBegin_Callback(on_OpenGL_TouchBegin);
	ccrSet_OpenGL_TouchMove_Callback(on_OpenGL_TouchMove);
	ccrSet_OpenGL_TouchEnd_Callback(on_OpenGL_TouchEnd);

	ccrBegin_OpenGL_Drawing();

	while (ccrGetEvent(false) != CCR_EVENT_QUIT);

	ccrEnd_OpenGL_Drawing();

	return 0;
}
