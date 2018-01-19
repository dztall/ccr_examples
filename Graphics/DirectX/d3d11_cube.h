struct ModelViewProjectionConstantBuffer
{
	float model[16];
	float view[16];
	float projection[16];
};

struct VertexPositionColor
{
	float pos[3];
	float color[3];
};

void Initialize_Sample();
void CreateWindowSizeDependentResources_Sample();
void CreateDeviceDependentResources_Sample();
void ReleaseDeviceDependentResources_Sample();
void TrackingUpdate_Sample(float positionX, float positionY);
void Update_Sample(double timeStepInSeconds);
void Render_Sample();
void Rotate_Sample(float radians);

//Sample code variables.
float g_pointerLocationX, g_pointerLocationY;
double totalTime;
ID3D11InputLayout *g_inputLayout;
ID3D11Buffer *g_vertexBuffer;
ID3D11Buffer *g_indexBuffer;
ID3D11VertexShader *g_vertexShader;
ID3D11PixelShader *g_pixelShader;
ID3D11Buffer *g_constantBuffer;
struct ModelViewProjectionConstantBuffer g_constantBufferData;
unsigned int g_indexCount;
float g_degreesPerSecond;
BOOL g_tracking;
float userRotationRadians;
float userPreviousPositionX;

void MatrixPerspectiveFovRH(float *out, float fovY, float aspect, float zn, float zf)
{
	float halfFovY = fovY / 2;
	float yScale = (float)(cos(halfFovY) / sin(halfFovY));
	float xScale = yScale / aspect;

	float temp[16] =
	{
		xScale,0,0,0,
		0,yScale,0,0,
		0,0,zf / (zn - zf),-1,
		0,0,zn*zf / (zn - zf),0
	};

	memcpy(out, temp, sizeof(temp));
}

void Vec3Normalize(
	float *out, const float *v)
{
	float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	out[0] = v[0] / length;
	out[1] = v[1] / length;
	out[2] = v[2] / length;
}

void Vec3Cross(
	float *out,
	const float *v1,
	const float *v2)
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

float Vec3Dot(
	const float *v1,
	const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void MatrixLookAtRH(float *out,
	const float *eye,
	const float *at,
	const float *up)
{
	float zaxis[3];
	zaxis[0] = (eye[0] - at[0]);
	zaxis[1] = (eye[1] - at[1]);
	zaxis[2] = (eye[2] - at[2]);
	Vec3Normalize(zaxis, zaxis);

	float xaxis[3];
	Vec3Cross(xaxis, up, zaxis);
	Vec3Normalize(xaxis, xaxis);

	float yaxis[3];
	Vec3Cross(yaxis, zaxis, xaxis);

	float temp[16] =
	{
		xaxis[0],yaxis[0],zaxis[0],0,
		xaxis[1],yaxis[1],zaxis[1],0,
		xaxis[2],yaxis[2],zaxis[2],0,
		-Vec3Dot(xaxis,eye),-Vec3Dot(yaxis,eye),-Vec3Dot(zaxis,eye),1
	};

	memcpy(out, temp, sizeof(temp));
}

void MatrixTranspose(
	float *out, float *m)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			out[j * 4 + i] = m[i * 4 + j];
}

void MatrixMultiply(
	float *out, const float *m1, const float *m2)
{
	int i, j, k;

	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j)
		{
			out[i * 4 + j] = 0.0f;
			for (k = 0; k < 4; ++k)
				out[i * 4 + j] = out[i * 4 + j] + m1[i * 4 + k] * m2[k * 4 + j];
		}
	}
}

void MatrixRotationY(
	float *out, float angle)
{
	float temp[16] =
	{
		(float)cos(angle),0,(float)-sin(angle),0,
		0,1,0,0,
		(float)sin(angle),0,(float)cos(angle),0,
		0,0,0,1
	};
	memcpy(out, temp, sizeof(temp));
}