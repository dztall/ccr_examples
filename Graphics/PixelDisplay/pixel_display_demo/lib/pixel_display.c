#include "pixel_display.h"

#ifdef _OS_OSX_
#define on_GL_Init on_OpenGL_Init
#define on_GL_Final on_OpenGL_Final
#define on_GL_Size on_OpenGL_Size
#define on_GL_Update on_OpenGL_Update
#define on_GL_Render on_OpenGL_Render
#define on_GL_TouchBegin on_OpenGL_TouchBegin
#define on_GL_TouchMove on_OpenGL_TouchMove
#define on_GL_TouchEnd on_OpenGL_TouchEnd
#define ccrBegin_GL_Drawing ccrBegin_OpenGL_Drawing
#define ccrEnd_GL_Drawing ccrEnd_OpenGL_Drawing
#define ccrSet_GL_Init_Callback ccrSet_OpenGL_Init_Callback
#define ccrSet_GL_Final_Callback ccrSet_OpenGL_Final_Callback
#define ccrSet_GL_Size_Callback ccrSet_OpenGL_Size_Callback
#define ccrSet_GL_Update_Callback ccrSet_OpenGL_Update_Callback
#define ccrSet_GL_Render_Callback ccrSet_OpenGL_Render_Callback
#define ccrSet_GL_TouchBegin_Callback ccrSet_OpenGL_TouchBegin_Callback
#define ccrSet_GL_TouchMove_Callback ccrSet_OpenGL_TouchMove_Callback
#define ccrSet_GL_TouchEnd_Callback ccrSet_OpenGL_TouchEnd_Callback
#else
#define on_GL_Init on_GLES2_Init
#define on_GL_Final on_GLES2_Final
#define on_GL_Size on_GLES2_Size
#define on_GL_Update on_GLES2_Update
#define on_GL_Render on_GLES2_Render
#define on_GL_TouchBegin on_GLES2_TouchBegin
#define on_GL_TouchMove on_GLES2_TouchMove
#define on_GL_TouchEnd on_GLES2_TouchEnd
#define ccrBegin_GL_Drawing ccrBegin_GLES2_Drawing
#define ccrEnd_GL_Drawing ccrEnd_GLES2_Drawing
#define ccrSet_GL_Init_Callback ccrSet_GLES2_Init_Callback
#define ccrSet_GL_Final_Callback ccrSet_GLES2_Final_Callback
#define ccrSet_GL_Size_Callback ccrSet_GLES2_Size_Callback
#define ccrSet_GL_Update_Callback ccrSet_GLES2_Update_Callback
#define ccrSet_GL_Render_Callback ccrSet_GLES2_Render_Callback
#define ccrSet_GL_TouchBegin_Callback ccrSet_GLES2_TouchBegin_Callback
#define ccrSet_GL_TouchMove_Callback ccrSet_GLES2_TouchMove_Callback
#define ccrSet_GL_TouchEnd_Callback ccrSet_GLES2_TouchEnd_Callback
#endif

GLuint simpleProgram;
int xPixels = 0;
pixel_display_update_callback_t user_pixel_display_update_callback = NULL;
pixel_display_resize_callback_t user_pixel_display_resize_callback = NULL;
int dirty_rectangle[4];

GLuint frameTexture = 0;
int frameTextureWidth = 0, frameTextureHeight = 0;
unsigned int *spriteData = NULL;

const float aVertices[6 * 3] =
{
	/* Front face. */
	/* Bottom left */
	-1,1,-0.5,
	1,-1,-0.5,
	-1,-1,-0.5,
	/* Top right */
	-1,1,-0.5,
	1,1,-0.5,
	1,-1,-0.5
};

const float aTexCoords[6 * 2] =
{
	/* Front face. */
	/* Bottom left */
	0,1,
	1,0,
	0,0,
	/* Top right */
	0,1,
	1,1,
	1,0,
};

#ifdef _OS_OSX_
const char *simpleVertexShader =
"attribute vec4 Position;"
"attribute vec2 TexCoordIn;"
"varying vec2 TexCoordOut;"
"void main(void)"
"{"
" gl_Position=Position;"
" TexCoordOut = TexCoordIn;"
"}";

const char *simpleFragmentShader =
"varying vec2 TexCoordOut;"
"uniform sampler2D Texture;"
"void main(void)"
"{"
" gl_FragColor=texture2D(Texture, TexCoordOut);"
"}";
#else
const char *simpleVertexShader =
"attribute vec4 Position;"
"attribute vec2 TexCoordIn;"
"varying vec2 TexCoordOut;"
"void main(void)"
"{"
" gl_Position=Position;"
" TexCoordOut = TexCoordIn;"
"}";

const char *simpleFragmentShader =
"varying lowp vec2 TexCoordOut;"
"uniform sampler2D Texture;"
"void main(void)"
"{"
" gl_FragColor=texture2D(Texture, TexCoordOut);"
"}";
#endif

GLuint BuildShader(const char *source, GLenum shaderType)
{
	GLuint shaderHandle = glCreateShader(shaderType);
	glShaderSource(shaderHandle, 1, &source, 0);
	glCompileShader(shaderHandle);
	GLint compileSuccess;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);

	if (compileSuccess == GL_FALSE)
	{
		GLchar messages[256];
		glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
		printf("compile glsl error : %s\n", messages);
	}

	return shaderHandle;
}

GLuint BuildProgram(const char *vShader, const char *fShader)
{
	GLuint vertexShader = BuildShader(vShader, GL_VERTEX_SHADER);
	GLuint fragmentShader = BuildShader(fShader, GL_FRAGMENT_SHADER);

	GLuint programHandle = glCreateProgram();
	glAttachShader(programHandle, vertexShader);
	glAttachShader(programHandle, fragmentShader);
	glLinkProgram(programHandle);

	GLint linkSuccess;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess == GL_FALSE)
	{
		GLchar messages[256];
		glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
		printf("compile glsl error : %s\n", messages);
	}

	return programHandle;
}

void RecreateFrameTexture(int view_w, int view_h)
{
	frameTextureWidth = xPixels;
	frameTextureHeight = frameTextureWidth * ((float)view_h / view_w);
	if (frameTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &frameTexture);
		frameTexture = 0;
	}

	glGenTextures(1, &frameTexture);
	glBindTexture(GL_TEXTURE_2D, frameTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	spriteData = calloc(frameTextureWidth*frameTextureHeight * 4, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameTextureWidth, frameTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
}

void ResetDirtyRectangle()
{
	dirty_rectangle[0] = 999999;
	dirty_rectangle[1] = 999999;
	dirty_rectangle[2] = -1;
	dirty_rectangle[3] = -1;
}

//OpenGLES2 handlers : init , final , update , render , touch-input
void on_GL_Init(int view_w, int view_h)
{
	simpleProgram = BuildProgram(simpleVertexShader, simpleFragmentShader);
	glUseProgram(simpleProgram);

	RecreateFrameTexture(view_w, view_h);
}

void on_GL_Final()
{
	if (simpleProgram)
		glDeleteProgram(simpleProgram);
	simpleProgram = 0;

	if (spriteData)
		free(spriteData);
	spriteData = NULL;
}

void on_GL_Size(int view_w, int view_h)
{
	glViewport(0, 0, view_w, view_h);

	RecreateFrameTexture(view_w, view_h);

	if (user_pixel_display_resize_callback)
		user_pixel_display_resize_callback(frameTextureWidth, frameTextureHeight);
}

void on_GL_Update(float timeStepInSeconds)
{
	if (user_pixel_display_update_callback)
		user_pixel_display_update_callback(timeStepInSeconds, frameTextureWidth, frameTextureHeight);
	if (dirty_rectangle[2] != -1)
	{
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frameTextureWidth, frameTextureHeight, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
	}
	ResetDirtyRectangle();
}

void on_GL_Render()
{
	GLuint positionSlot = glGetAttribLocation(simpleProgram, "Position");
	GLuint texCoordInSlot = glGetAttribLocation(simpleProgram, "TexCoordIn");

	GLuint textureUniform = glGetUniformLocation(simpleProgram, "Texture");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glUniform1i(textureUniform, 0);

	glEnableVertexAttribArray(positionSlot);
	glEnableVertexAttribArray(texCoordInSlot);

	glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, aVertices);
	glVertexAttribPointer(texCoordInSlot, 2, GL_FLOAT, GL_FALSE, 0, aTexCoords);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(texCoordInSlot);

	assert(glGetError() == GL_NO_ERROR);
}

void on_GL_TouchBegin(float x, float y)
{
}

void on_GL_TouchMove(float prev_x, float prev_y, float x, float y)
{
}

void on_GL_TouchEnd(float x, float y)
{
}

void pixel_display_init(int width, pixel_display_update_callback_t pixel_display_update_callback, pixel_display_resize_callback_t pixel_display_resize_callback)
{
	xPixels = width;
	user_pixel_display_update_callback = pixel_display_update_callback;
	user_pixel_display_resize_callback = pixel_display_resize_callback;

	ccrSet_GL_Init_Callback(on_GL_Init);
	ccrSet_GL_Final_Callback(on_GL_Final);
	ccrSet_GL_Size_Callback(on_GL_Size);
	ccrSet_GL_Update_Callback(on_GL_Update);
	ccrSet_GL_Render_Callback(on_GL_Render);
	ccrSet_GL_TouchBegin_Callback(on_GL_TouchBegin);
	ccrSet_GL_TouchMove_Callback(on_GL_TouchMove);
	ccrSet_GL_TouchEnd_Callback(on_GL_TouchEnd);

	ccrBegin_GL_Drawing();

	ResetDirtyRectangle();
}

void pixel_display_enter_loop()
{
	while (ccrGetEvent(false) != CCR_EVENT_QUIT);
}

void pixel_display_finish()
{
	ccrEnd_GL_Drawing();
}

void pixel_display_set_pixel(int x, int y, int r255, int g255, int b255)
{
	if (x < 0 || y < 0 || x >= frameTextureWidth || y >= frameTextureHeight)
	{
		printf("pixel_display_set_pixel(%d,%d), Error pixel out of range.\n", x, y);
		return;
	}

	y = frameTextureHeight - y - 1;
	unsigned int color = 0xff000000;
	color |= r255 & 0x000000ff;
	color |= (g255 & 0x000000ff) << 8;
	color |= (b255 & 0x000000ff) << 16;
	spriteData[y*frameTextureWidth + x] = color;

	if (x < dirty_rectangle[0])
		dirty_rectangle[0] = x;
	if (y < dirty_rectangle[1])
		dirty_rectangle[1] = y;
	if ((x + 1) > dirty_rectangle[2])
		dirty_rectangle[2] = x + 1;
	if ((y + 1) > dirty_rectangle[3])
		dirty_rectangle[3] = y + 1;
}

void pixel_display_clear_pixel()
{
	memset(spriteData, 255, frameTextureWidth*frameTextureHeight * 4);
	dirty_rectangle[0] = 0;
	dirty_rectangle[1] = 0;
	dirty_rectangle[2] = frameTextureWidth;
	dirty_rectangle[3] = frameTextureHeight;
}
