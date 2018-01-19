#if !defined(_OS_OSX_) && !defined(_OS_WINDOWS_)
#error "Not supported platform!"
#endif

#include <stdio.h>
#include <math.h>

#if defined(_OS_OSX_)
#include <OpenGL/gl.h>
#elif defined(_OS_WINDOWS_)
#include <GL/glew.h>
#endif

#include <ccr.h>

GLuint simpleProgram;

/* 3D data. Vertex range -0.5..0.5 in all axes.
* Z -0.5 is near, 0.5 is far. */
const float aVertices[36 * 3] =
{
	/* Front face. */
	/* Bottom left */
	-0.5,0.5,-0.5,
	0.5,-0.5,-0.5,
	-0.5,-0.5,-0.5,
	/* Top right */
	-0.5,0.5,-0.5,
	0.5,0.5,-0.5,
	0.5,-0.5,-0.5,
	/* Left face */
	/* Bottom left */
	-0.5,0.5,0.5,
	-0.5,-0.5,-0.5,
	-0.5,-0.5,0.5,
	/* Top right */
	-0.5,0.5,0.5,
	-0.5,0.5,-0.5,
	-0.5,-0.5,-0.5,
	/* Top face */
	/* Bottom left */
	-0.5,0.5,0.5,
	0.5,0.5,-0.5,
	-0.5,0.5,-0.5,
	/* Top right */
	-0.5,0.5,0.5,
	0.5,0.5,0.5,
	0.5,0.5,-0.5,
	/* Right face */
	/* Bottom left */
	0.5,0.5,-0.5,
	0.5,-0.5,0.5,
	0.5,-0.5,-0.5,
	/* Top right */
	0.5,0.5,-0.5,
	0.5,0.5,0.5,
	0.5,-0.5,0.5,
	/* Back face */
	/* Bottom left */
	0.5,0.5,0.5,
	-0.5,-0.5,0.5,
	0.5,-0.5,0.5,
	/* Top right */
	0.5,0.5,0.5,
	-0.5,0.5,0.5,
	-0.5,-0.5,0.5,
	/* Bottom face */
	/* Bottom left */
	-0.5,-0.5,-0.5,
	0.5,-0.5,0.5,
	-0.5,-0.5,0.5,
	/* Top right */
	-0.5,-0.5,-0.5,
	0.5,-0.5,-0.5,
	0.5,-0.5,0.5,
};

const float aColours[36 * 3] =
{
	/* Front face */
	/* Bottom left */
	1.0,0.0,0.0, /* red */
	0.0,0.0,1.0, /* blue */
	0.0,1.0,0.0, /* green */
	/* Top right */
	1.0,0.0,0.0, /* red */
	1.0,1.0,0.0, /* yellow */
	0.0,0.0,1.0, /* blue */
	/* Left face */
	/* Bottom left */
	1.0,1.0,1.0, /* white */
	0.0,1.0,0.0, /* green */
	0.0,1.0,1.0, /* cyan */
	/* Top right */
	1.0,1.0,1.0, /* white */
	1.0,0.0,0.0, /* red */
	0.0,1.0,0.0, /* green */
	/* Top face */
	/* Bottom left */
	1.0,1.0,1.0, /* white */
	1.0,1.0,0.0, /* yellow */
	1.0,0.0,0.0, /* red */
	/* Top right */
	1.0,1.0,1.0, /* white */
	0.0,0.0,0.0, /* black */
	1.0,1.0,0.0, /* yellow */
	/* Right face */
	/* Bottom left */
	1.0,1.0,0.0, /* yellow */
	1.0,0.0,1.0, /* magenta */
	0.0,0.0,1.0, /* blue */
	/* Top right */
	1.0,1.0,0.0, /* yellow */
	0.0,0.0,0.0, /* black */
	1.0,0.0,1.0, /* magenta */
	/* Back face */
	/* Bottom left */
	0.0,0.0,0.0, /* black */
	0.0,1.0,1.0, /* cyan */
	1.0,0.0,1.0, /* magenta */
	/* Top right */
	0.0,0.0,0.0, /* black */
	1.0,1.0,1.0, /* white */
	0.0,1.0,1.0, /* cyan */
	/* Bottom face */
	/* Bottom left */
	0.0,1.0,0.0, /* green */
	1.0,0.0,1.0, /* magenta */
	0.0,1.0,1.0, /* cyan */
	/* Top right */
	0.0,1.0,0.0, /* green */
	0.0,0.0,1.0, /* blue */
	1.0,0.0,1.0, /* magenta */
};

const char *SimpleVertexShader =
"attribute vec4 Position;"
"attribute vec4 SourceColor;"
"varying vec4 DestinationColor;"
"uniform mat4 Modelview;"
"uniform mat4 Projection;"
"void main(void)"
"{"
" DestinationColor=SourceColor;"
" gl_Position=Projection*Modelview*Position;"
"}";

const char *SimpleFragmentShader =
"varying vec4 DestinationColor;"
"void main(void)"
"{"
" gl_FragColor=DestinationColor;"
"}";

void multiply_matrix(float *A, float *B, float *C)
{
	int i, j, k;
	float aTmp[16];

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			aTmp[j * 4 + i] = 0.0;

			for (k = 0; k < 4; k++)
				aTmp[j * 4 + i] += A[k * 4 + i] * B[j * 4 + k];
		}
	}

	for (i = 0; i < 16; i++)
		C[i] = aTmp[i];
}

void rotate_matrix(double angle, double x, double y, double z, float *R)
{
	double radians, c, s, c1, u[3], length;
	int i, j;

	radians = (angle * 3.14) / 180.0;

	c = cos(radians);
	s = sin(radians);

	c1 = 1.0 - cos(radians);

	length = sqrt(x * x + y * y + z * z);

	u[0] = x / length;
	u[1] = y / length;
	u[2] = z / length;

	for (i = 0; i < 16; i++)
		R[i] = 0.0;

	R[15] = 1.0;

	for (i = 0; i < 3; i++)
	{
		R[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
		R[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
			R[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
	}
}

void perspective_matrix(double fovy, double aspect, double znear, double zfar, float *P)
{
	int i;
	double f;

	f = 1.0 / tan(fovy * 0.5);

	for (i = 0; i < 16; i++)
		P[i] = 0.0;

	P[0] = f / aspect;
	P[5] = f;
	P[10] = (znear + zfar) / (znear - zfar);
	P[11] = -1.0;
	P[14] = (2.0 * znear * zfar) / (znear - zfar);
	P[15] = 0.0;
}

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

//OpenGL handlers : init , final , update , render , input
void on_OpenGL_Init(int view_w, int view_h)
{
#if defined(_OS_WINDOWS_)
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	simpleProgram = BuildProgram(SimpleVertexShader, SimpleFragmentShader);
	glUseProgram(simpleProgram);
}

void on_OpenGL_Final()
{
	if (simpleProgram)
		glDeleteProgram(simpleProgram);
	simpleProgram = 0;
}

float aspect = 1.0f;
void on_OpenGL_Size(int view_w, int view_h)
{
	glViewport(0, 0, view_w, view_h);
	aspect = view_w / (float)view_h;
}

float iXangle = 0, iYangle = 0, iZangle = 0;
void on_OpenGL_Update(float timeStep_sec)
{
	iXangle += 0.9*timeStep_sec * 30;
	iYangle += 0.7*timeStep_sec * 30;
	iZangle += 0.5*timeStep_sec * 30;

	if (iXangle >= 360)
		iXangle -= 360;
	if (iXangle < 0)
		iXangle += 360;
	if (iYangle >= 360)
		iYangle -= 360;
	if (iYangle < 0)
		iYangle += 360;
	if (iZangle >= 360)
		iZangle -= 360;
	if (iZangle < 0)
		iZangle += 360;
}

void on_OpenGL_Render()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float modelViewMatrix[16] =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	rotate_matrix(iXangle, 1.0, 0.0, 0.0, modelViewMatrix);
	float aRotate[16];
	rotate_matrix(iYangle, 0.0, 1.0, 0.0, aRotate);
	multiply_matrix(aRotate, modelViewMatrix, modelViewMatrix);
	rotate_matrix(iZangle, 0.0, 1.0, 0.0, aRotate);
	multiply_matrix(aRotate, modelViewMatrix, modelViewMatrix);
	modelViewMatrix[14] -= 2.5;
	GLint modelviewUniform = glGetUniformLocation(simpleProgram, "Modelview");
	glUniformMatrix4fv(modelviewUniform, 1, 0, &modelViewMatrix[0]);

	GLuint positionSlot = glGetAttribLocation(simpleProgram, "Position");
	GLuint colorSlot = glGetAttribLocation(simpleProgram, "SourceColor");

	glEnableVertexAttribArray(positionSlot);
	glEnableVertexAttribArray(colorSlot);

	glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, aVertices);
	glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, aColours);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	GLint projUniform = glGetUniformLocation(simpleProgram, "Projection");
	float aPerspective[16];
	perspective_matrix(1.3f, aspect, 0.01, 100.0, aPerspective);
	glUniformMatrix4fv(projUniform, 1, GL_FALSE, aPerspective);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(colorSlot);
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
