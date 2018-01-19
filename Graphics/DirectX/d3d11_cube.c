#if !defined(_WIN32)
#error "Not supported platform!"
#else

#include <winapifamily.h>

#if (WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP) && (WINAPI_FAMILY != WINAPI_FAMILY_PC_APP)
#error "Not supported platform!"
#else

#include <stdio.h>
#include <math.h>

#define INITGUID

#include <D3DCompiler.h>
#include <d3d11_2.h>

#include "d3d11_base.h"
#include "d3d11_cube.h"

void Initialize_Sample()
{
	g_degreesPerSecond = 30;
	g_indexCount = 0;
	g_tracking = FALSE;
	g_inputLayout = NULL;
	g_vertexBuffer = NULL;
	g_indexBuffer = NULL;
	g_vertexShader = NULL;
	g_pixelShader = NULL;
	g_constantBuffer = NULL;
	totalTime = 0.0;

	userRotationRadians = 0;

	CreateDeviceDependentResources_Sample();
	CreateWindowSizeDependentResources_Sample();
}

void Finalize_Sample()
{
	ReleaseDeviceDependentResources_Sample();
}

void Update_Sample(double timeStepInSeconds)
{
	TrackingUpdate_Sample(g_pointerLocationX, g_pointerLocationY);

	totalTime += timeStepInSeconds;

	float radiansPerSecond = (float)(0.0174532925*g_degreesPerSecond);
	double totalRotation = totalTime * radiansPerSecond;
	float radians = (float)(fmod(totalRotation, 2 * 3.141592f));

	Rotate_Sample(-radians + userRotationRadians);
}

void CreateWindowSizeDependentResources_Sample()
{
	float aspectRatio = g_outputSizeWidth / g_outputSizeHeight;
	float fovAngleY = 50.0f * 3.141592f / 180.0f;

	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;

	float perspectiveMatrix[16];
	MatrixPerspectiveFovRH((float *)perspectiveMatrix,
		fovAngleY, aspectRatio, 0.01f, 100.0f);

	float projectionMatrix[16];
	MatrixMultiply(projectionMatrix, perspectiveMatrix,
		GetOrientationTransform_Base());
	float projectionMatrixTransposed[16];
	MatrixTranspose(projectionMatrixTransposed, projectionMatrix);

	memcpy(&g_constantBufferData.projection,
		projectionMatrixTransposed,
		sizeof(g_constantBufferData.projection));

	static const float eye[4] = { 0.0f,0.7f,1.5f,0.0f };
	static const float at[4] = { 0.0f,-0.1f,0.0f,0.0f };
	static const float up[4] = { 0.0f,1.0f,0.0f,0.0f };

	float lookAtMatrix[16];
	MatrixLookAtRH(lookAtMatrix, eye, at, up);
	float lookAtMatrixTransposed[16];
	MatrixTranspose(lookAtMatrixTransposed, lookAtMatrix);
	memcpy(&g_constantBufferData.view,
		lookAtMatrixTransposed,
		sizeof(g_constantBufferData.view));
}

void Render_Sample()
{
	g_d3dContext->lpVtbl->UpdateSubresource(g_d3dContext,
		(ID3D11Resource *)g_constantBuffer, 0, NULL, &g_constantBufferData, 0, 0);

	UINT stride = sizeof(struct VertexPositionColor);
	UINT offset = 0;
	g_d3dContext->lpVtbl->IASetVertexBuffers(g_d3dContext, 0, 1, &g_vertexBuffer, &stride, &offset);
	g_d3dContext->lpVtbl->IASetIndexBuffer(g_d3dContext, g_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_d3dContext->lpVtbl->IASetPrimitiveTopology(g_d3dContext, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_d3dContext->lpVtbl->IASetInputLayout(g_d3dContext, g_inputLayout);
	g_d3dContext->lpVtbl->VSSetShader(g_d3dContext, g_vertexShader, NULL, 0);
	g_d3dContext->lpVtbl->VSSetConstantBuffers(g_d3dContext, 0, 1, &g_constantBuffer);
	g_d3dContext->lpVtbl->PSSetShader(g_d3dContext, g_pixelShader, NULL, 0);
	g_d3dContext->lpVtbl->DrawIndexed(g_d3dContext, g_indexCount, 0, 0);
}

void Rotate_Sample(float radians)
{
	float worldMatrix[16];
	MatrixRotationY(worldMatrix, radians);
	float worldMatrixTransposed[16];
	MatrixTranspose(worldMatrixTransposed, worldMatrix);
	memcpy(&g_constantBufferData.model, worldMatrixTransposed,
		sizeof(g_constantBufferData.model));
}

void StartTracking_Sample(float positionX, float positionY)
{
	g_tracking = TRUE;
	userPreviousPositionX = positionX;
}

void TrackingUpdate_Sample(float positionX, float positionY)
{
	if (g_tracking)
	{
		float dx = positionX - userPreviousPositionX;
		userRotationRadians += 3.141592f * dx / g_outputSizeWidth;
		userPreviousPositionX = positionX;
	}
}

void StopTracking_Sample(float positionX, float positionY)
{
	g_tracking = FALSE;
}

void OnDeviceLost_Sample()
{
	ReleaseDeviceDependentResources_Sample();
}

void OnDeviceRestored_Sample()
{
	CreateDeviceDependentResources_Sample();
	CreateWindowSizeDependentResources_Sample();
}

void CreateDeviceDependentResources_Sample()
{
	HRESULT hr = S_OK;

	//Create vertex shader.

	const char sampleVertexShaderSource[] =
		"cbuffer ModelViewProjectionConstantBuffer : register(b0)\n"
		"{\n"
		"	matrix model;\n"
		"	matrix view;\n"
		"	matrix projection;\n"
		"};\n"
		"\n"
		"struct VertexShaderInput\n"
		"{\n"
		"	float3 pos : POSITION;\n"
		"	float3 color : COLOR0;\n"
		"};\n"
		"\n"
		"struct PixelShaderInput\n"
		"{\n"
		"	float4 pos : SV_POSITION;\n"
		"	float3 color : COLOR0;\n"
		"};\n"
		"\n"
		"PixelShaderInput main(VertexShaderInput input)\n"
		"{\n"
		"	PixelShaderInput output;\n"
		"	float4 pos=float4(input.pos,1.0f);\n"
		"\n"
		"	pos=mul(pos,model);\n"
		"	pos=mul(pos,view);\n"
		"	pos=mul(pos,projection);\n"
		"	output.pos=pos;\n"
		"\n"
		"	output.color=input.color;\n"
		"\n"
		"	return output;\n"
		"}";

	ID3DBlob *sampleVertexShaderObjectBlob = NULL;
	ID3DBlob *sampleVertexShaderErrorMessageBlob = NULL;
	hr = D3DCompile2(sampleVertexShaderSource, sizeof(sampleVertexShaderSource),
		"sampleVertexShaderSource", NULL, NULL, "main",
		"vs_4_0_level_9_3", 0, 0, 0, NULL, 0, &sampleVertexShaderObjectBlob,
		&sampleVertexShaderErrorMessageBlob);

	if (FAILED(hr))
	{
		const char *errorMessage = (const char *)
			sampleVertexShaderErrorMessageBlob->lpVtbl->GetBufferPointer(
				sampleVertexShaderErrorMessageBlob);
		printf("Error compiling vertex shader : %s\n", errorMessage);
	}

	if (sampleVertexShaderErrorMessageBlob)
		sampleVertexShaderErrorMessageBlob->lpVtbl->Release(
			sampleVertexShaderErrorMessageBlob);
	sampleVertexShaderErrorMessageBlob = NULL;

	if (FAILED(hr))
		return;

	g_d3dDevice->lpVtbl->CreateVertexShader(
		g_d3dDevice,
		sampleVertexShaderObjectBlob->lpVtbl->GetBufferPointer(
			sampleVertexShaderObjectBlob),
		sampleVertexShaderObjectBlob->lpVtbl->GetBufferSize(
			sampleVertexShaderObjectBlob),
		NULL,
		&g_vertexShader);

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	g_d3dDevice->lpVtbl->CreateInputLayout(
		g_d3dDevice,
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		sampleVertexShaderObjectBlob->lpVtbl->GetBufferPointer(
			sampleVertexShaderObjectBlob),
		sampleVertexShaderObjectBlob->lpVtbl->GetBufferSize(
			sampleVertexShaderObjectBlob),
		&g_inputLayout);

	if (sampleVertexShaderObjectBlob)
		sampleVertexShaderObjectBlob->lpVtbl->Release(
			sampleVertexShaderObjectBlob);
	sampleVertexShaderObjectBlob = NULL;

	//Create pixel shader.

	const char samplePixelShaderSource[] =
		"struct PixelShaderInput\n"
		"{\n"
		"	float4 pos : SV_POSITION;\n"
		"	float3 color : COLOR0;\n"
		"};\n"
		"\n"
		"float4 main(PixelShaderInput input): SV_TARGET\n"
		"{\n"
		"	return float4(input.color,1.0f);\n"
		"}";

	ID3DBlob *samplePixelShaderObjectBlob = NULL;
	ID3DBlob *samplePixelShaderErrorMessageBlob = NULL;
	hr = D3DCompile2(samplePixelShaderSource, sizeof(samplePixelShaderSource),
		"samplePixelShaderSource", NULL, NULL, "main",
		"ps_4_0_level_9_3", 0, 0, 0, NULL, 0, &samplePixelShaderObjectBlob,
		&samplePixelShaderErrorMessageBlob);

	if (FAILED(hr))
	{
		const char *errorMessage =
			(const char *)samplePixelShaderErrorMessageBlob->lpVtbl->
			GetBufferPointer(samplePixelShaderErrorMessageBlob);
		printf("Error compiling pixel shader : %s\n", errorMessage);
	}

	if (samplePixelShaderErrorMessageBlob)
		samplePixelShaderErrorMessageBlob->lpVtbl->Release(
			samplePixelShaderErrorMessageBlob);
	samplePixelShaderErrorMessageBlob = NULL;

	if (FAILED(hr))
		return;

	g_d3dDevice->lpVtbl->CreatePixelShader(
		g_d3dDevice,
		samplePixelShaderObjectBlob->lpVtbl->GetBufferPointer(
			samplePixelShaderObjectBlob),
		samplePixelShaderObjectBlob->lpVtbl->GetBufferSize(
			samplePixelShaderObjectBlob),
		NULL,
		&g_pixelShader);

	if (samplePixelShaderObjectBlob)
		samplePixelShaderObjectBlob->lpVtbl->Release(samplePixelShaderObjectBlob);
	samplePixelShaderObjectBlob = NULL;

	D3D11_BUFFER_DESC constantBufferDesc =
	{
		sizeof(struct ModelViewProjectionConstantBuffer),0,
		D3D11_BIND_CONSTANT_BUFFER,0,0,0
	};
	hr = g_d3dDevice->lpVtbl->CreateBuffer(
		g_d3dDevice,
		&constantBufferDesc,
		NULL,
		&g_constantBuffer);

	if (FAILED(hr))
	{
		printf("Error creating constant buffer : hr=%d\n", hr);
		return;
	}

	//Create vertex/index buffer for cube mesh.
	static const struct VertexPositionColor cubeVertices[] =
	{
		{{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,0.0f}},
		{{-0.5f,-0.5f,0.5f},{0.0f,0.0f,1.0f}},
		{{-0.5f,0.5f,-0.5f},{0.0f,1.0f,0.0f}},
		{{-0.5f,0.5f,0.5f},{0.0f,1.0f,1.0f}},
		{{0.5f,-0.5f,-0.5f},{1.0f,0.0f,0.0f}},
		{{0.5f,-0.5f,0.5f},{1.0f,0.0f,1.0f}},
		{{0.5f,0.5f,-0.5f},{1.0f,1.0f,0.0f}},
		{{0.5f,0.5f,0.5f},{1.0f,1.0f,1.0f}},
	};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	D3D11_BUFFER_DESC vertexBufferDesc =
	{
		sizeof(cubeVertices),D3D11_USAGE_DEFAULT,D3D11_BIND_VERTEX_BUFFER,0,0,0
	};
	g_d3dDevice->lpVtbl->CreateBuffer(
		g_d3dDevice,
		&vertexBufferDesc,
		&vertexBufferData,
		&g_vertexBuffer);

	static const unsigned short cubeIndices[] =
	{
		0,2,1,// -x
		1,2,3,
		4,5,6,// +x
		5,7,6,
		0,1,5,// -y
		0,5,4,
		2,6,7,// +y
		2,7,3,
		0,4,6,// -z
		0,6,2,
		1,3,7,// +z
		1,7,5,
	};

	g_indexCount = ARRAYSIZE(cubeIndices);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	D3D11_BUFFER_DESC indexBufferDesc =
	{
		sizeof(cubeIndices),0,D3D11_BIND_INDEX_BUFFER,0,0,0
	};
	g_d3dDevice->lpVtbl->CreateBuffer(g_d3dDevice,
		&indexBufferDesc, &indexBufferData, &g_indexBuffer);
}

void ReleaseDeviceDependentResources_Sample()
{
	if (g_vertexShader)
		g_vertexShader->lpVtbl->Release(g_vertexShader);
	g_vertexShader = NULL;

	if (g_inputLayout)
		g_inputLayout->lpVtbl->Release(g_inputLayout);
	g_inputLayout = NULL;

	if (g_pixelShader)
		g_pixelShader->lpVtbl->Release(g_pixelShader);
	g_pixelShader = NULL;

	if (g_constantBuffer)
		g_constantBuffer->lpVtbl->Release(g_constantBuffer);
	g_constantBuffer = NULL;

	if (g_vertexBuffer)
		g_vertexBuffer->lpVtbl->Release(g_vertexBuffer);
	g_vertexBuffer = NULL;

	if (g_indexBuffer)
		g_indexBuffer->lpVtbl->Release(g_indexBuffer);
	g_indexBuffer = NULL;
}

void on_D3D11_Init(float logicalSizeWidth, float logicalSizeHeight,
	int nativeOrientation, int currentOrientation,
	float compositionScaleX, float compositionScaleY, float dpi)
{
	Initialize_Base(logicalSizeWidth, logicalSizeHeight,
		nativeOrientation, currentOrientation, compositionScaleX,
		compositionScaleY, dpi, OnDeviceLost_Sample, OnDeviceRestored_Sample,
		CreateWindowSizeDependentResources_Sample);
	Initialize_Sample();
}

void on_D3D11_Final()
{
	Finalize_Sample();
	Finalize_Base();
}

void on_D3D11_Size(float logicalSizeWidth, float logicalSizeHeight)
{
	SetLogicalSize_Base(logicalSizeWidth, logicalSizeHeight);
}

void on_D3D11_Update(double timeStepInSeconds)
{
	Update_Sample(timeStepInSeconds);
}

void on_D3D11_Render()
{
	Render_Base(Render_Sample);
}

void on_D3D11_TouchBegin(float x, float y)
{
	StartTracking_Sample(x, y);
}

void on_D3D11_TouchMove(float x, float y)
{
	TrackingUpdate_Sample(x, y);
}

void on_D3D11_TouchEnd(float x, float y)
{
	StopTracking_Sample(x, y);
}

void on_D3D11_DeviceRotate(int orientation)
{
	SetCurrentOrientation_Base(orientation);
}

void on_D3D11_DPIChanged(float dpi)
{
	SetDpi_Base(dpi);
}

void on_D3D11_DisplayContentsInvalidated()
{
	ValidateDevice_Base();
}

void on_D3D11_CompositionScaleChanged(float scaleX, float scaleY)
{
	SetCompositionScale_Base(scaleX, scaleY);
}

void *get_D3D11_Device2()
{
	return GetD3D11Device2_Base();
}

void *get_D3D11_SwapChain1()
{
	return GetD3D11SwapChain1_Base();
}

#endif

#endif