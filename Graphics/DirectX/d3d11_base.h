void CreateDeviceResources_Base();
void CreateWindowSizeDependentResources_Base();
void Present_Base();
DXGI_MODE_ROTATION ComputeDisplayRotation_Base();
void HandleDeviceLost_Base(void(*onDeviceLostFn)(), void(*onDeviceRestoredFn)());
void ValidateDevice_Base();

enum Orientations
{
	DisplayOrientations_None = 0,
	DisplayOrientations_Landscape = 1,
	DisplayOrientations_Portrait = 2,
	DisplayOrientations_LandscapeFlipped = 4,
	DisplayOrientations_PortraitFlipped = 8,
};

//D3D11 objects.
IDXGISwapChain1 *g_swapChain;
ID3D11Device2 *g_d3dDevice;
ID3D11DeviceContext2 *g_d3dContext;
ID3D11RenderTargetView *g_d3dRenderTargetView;
ID3D11DepthStencilView *g_d3dDepthStencilView;
D3D11_VIEWPORT g_screenViewport;
D3D_FEATURE_LEVEL g_d3dFeatureLevel;

float g_orientationTransform3D[16];
float g_d3dRenderTargetSizeWidth, g_d3dRenderTargetSizeHeight;
float g_outputSizeWidth, g_outputSizeHeight;
float g_logicalSizeWidth, g_logicalSizeHeight;
enum Orientations g_nativeOrientation;
enum Orientations g_currentOrientation;
float g_dpi;
float g_compositionScaleX;
float g_compositionScaleY;

void(*onDeviceLostFn)() = NULL;
void(*onDeviceRestoredFn)() = NULL;
void(*CreateWindowSizeDependentResourcesFn)() = NULL;

void Initialize_Base(float logicalSizeWidth, float logicalSizeHeight, int nativeOrientation,
	int currentOrientation, float compositionScaleX, float compositionScaleY, float dpi,
	void(*onDeviceLostFnIn)(), void(*onDeviceRestoredFnIn)(), void(*CreateWindowSizeDependentResourcesFnIn)())
{
	onDeviceLostFn = onDeviceLostFnIn;
	onDeviceRestoredFn = onDeviceRestoredFnIn;
	CreateWindowSizeDependentResourcesFn = CreateWindowSizeDependentResourcesFnIn;

	g_d3dFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	g_d3dDevice = NULL;
	g_d3dContext = NULL;
	g_swapChain = NULL;
	g_d3dRenderTargetView = NULL;
	g_d3dDepthStencilView = NULL;

	g_logicalSizeWidth = logicalSizeWidth;
	g_logicalSizeHeight = logicalSizeHeight;
	g_nativeOrientation = (enum Orientations)nativeOrientation;
	g_currentOrientation = (enum Orientations)currentOrientation;
	g_dpi = dpi;
	g_compositionScaleX = compositionScaleX;
	g_compositionScaleY = compositionScaleY;

	CreateDeviceResources_Base();
	CreateWindowSizeDependentResources_Base();
}

void Finalize_Base()
{
	if (g_d3dDevice)
		g_d3dDevice->lpVtbl->Release(g_d3dDevice);
	if (g_d3dContext)
		g_d3dContext->lpVtbl->Release(g_d3dContext);
	if (g_swapChain)
		g_swapChain->lpVtbl->Release(g_swapChain);

	if (g_d3dRenderTargetView)
		g_d3dRenderTargetView->lpVtbl->Release(g_d3dRenderTargetView);
	if (g_d3dDepthStencilView)
		g_d3dDepthStencilView->lpVtbl->Release(g_d3dDepthStencilView);
}

BOOL Render_Base(void(*UserRenderFn)())
{
	g_d3dContext->lpVtbl->RSSetViewports(g_d3dContext, 1, &g_screenViewport);

	ID3D11RenderTargetView *const targets[1] = { g_d3dRenderTargetView };
	g_d3dContext->lpVtbl->OMSetRenderTargets(g_d3dContext, 1, targets, g_d3dDepthStencilView);

	const float color[] = { 0.392156899f,0.584313750f,0.929411829f,1.000000000f };
	g_d3dContext->lpVtbl->ClearRenderTargetView(g_d3dContext, g_d3dRenderTargetView, color);
	g_d3dContext->lpVtbl->ClearDepthStencilView(g_d3dContext, g_d3dDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	UserRenderFn();

	Present_Base();

	return TRUE;
}

void CreateDeviceResources_Base()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	ID3D11Device *device = NULL;
	ID3D11DeviceContext *context = NULL;

	HRESULT hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&device,
		&g_d3dFeatureLevel,
		&context);

	if (FAILED(hr))
	{
		D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_WARP,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			&g_d3dFeatureLevel,
			&context);
	}

	device->lpVtbl->QueryInterface(device, &IID_ID3D11Device2, &g_d3dDevice);
	device->lpVtbl->Release(device);

	context->lpVtbl->QueryInterface(context, &IID_ID3D11DeviceContext2, &g_d3dContext);
	context->lpVtbl->Release(context);

	IDXGIDevice3 *dxgiDevice = NULL;
	g_d3dDevice->lpVtbl->QueryInterface(g_d3dDevice, &IID_IDXGIDevice3, &dxgiDevice);
	dxgiDevice->lpVtbl->Release(dxgiDevice);
}

void CreateWindowSizeDependentResources_Base()
{
	ID3D11RenderTargetView *nullViews[] = { NULL };
	g_d3dContext->lpVtbl->OMSetRenderTargets(g_d3dContext, ARRAYSIZE(nullViews), nullViews, NULL);

	if (g_d3dRenderTargetView)
		g_d3dRenderTargetView->lpVtbl->Release(g_d3dRenderTargetView);
	g_d3dRenderTargetView = NULL;

	if (g_d3dDepthStencilView)
		g_d3dDepthStencilView->lpVtbl->Release(g_d3dDepthStencilView);
	g_d3dDepthStencilView = NULL;

	g_d3dContext->lpVtbl->Flush(g_d3dContext);

	g_outputSizeWidth = g_logicalSizeWidth * g_compositionScaleX;
	g_outputSizeHeight = g_logicalSizeHeight * g_compositionScaleY;

	g_outputSizeWidth = max(g_outputSizeWidth, 1);
	g_outputSizeHeight = max(g_outputSizeHeight, 1);

	DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation_Base();

	BOOL swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	g_d3dRenderTargetSizeWidth = swapDimensions ? g_outputSizeHeight : g_outputSizeWidth;
	g_d3dRenderTargetSizeHeight = swapDimensions ? g_outputSizeWidth : g_outputSizeHeight;

	if (g_swapChain != NULL)
	{
		HRESULT hr = g_swapChain->lpVtbl->ResizeBuffers(
			g_swapChain,
			2,
			lround(g_d3dRenderTargetSizeWidth),
			lround(g_d3dRenderTargetSizeHeight),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost_Base();
			return;
		}
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width = lround(g_d3dRenderTargetSizeWidth);
		swapChainDesc.Height = lround(g_d3dRenderTargetSizeHeight);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		IDXGIDevice3 *dxgiDevice = NULL;
		g_d3dDevice->lpVtbl->QueryInterface(g_d3dDevice, &IID_IDXGIDevice3, &dxgiDevice);

		IDXGIAdapter *dxgiAdapter = NULL;
		dxgiDevice->lpVtbl->GetAdapter(dxgiDevice, &dxgiAdapter);

		IDXGIFactory2 *dxgiFactory = NULL;
		dxgiAdapter->lpVtbl->GetParent(dxgiAdapter, &IID_IDXGIFactory2, &dxgiFactory);
		dxgiAdapter->lpVtbl->Release(dxgiAdapter);

		dxgiFactory->lpVtbl->CreateSwapChainForComposition(
			dxgiFactory,
			(IUnknown *)g_d3dDevice,
			&swapChainDesc,
			NULL,
			&g_swapChain);

		dxgiFactory->lpVtbl->Release(dxgiFactory);

		dxgiDevice->lpVtbl->SetMaximumFrameLatency(dxgiDevice, 1);
		dxgiDevice->lpVtbl->Release(dxgiDevice);
	}

	// Rotate 0 degree z-axis.
	static const float Rotation0[16] =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	// Rotate 90 degree z-axis.
	static const float Rotation90[16] =
	{
		0.0f,1.0f,0.0f,0.0f,
		-1.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	// Rotate 180 degree z-axis.
	static const float Rotation180[16] =
	{
		-1.0f,0.0f,0.0f,0.0f,
		0.0f,-1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	// Rotate 270 degree z-axis.
	static const float Rotation270[16] =
	{
		0.0f,-1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};

	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		memcpy(g_orientationTransform3D, Rotation0, sizeof(g_orientationTransform3D));
		break;

	case DXGI_MODE_ROTATION_ROTATE90:
		memcpy(g_orientationTransform3D, Rotation270, sizeof(g_orientationTransform3D));
		break;

	case DXGI_MODE_ROTATION_ROTATE180:
		memcpy(g_orientationTransform3D, Rotation180, sizeof(g_orientationTransform3D));
		break;

	case DXGI_MODE_ROTATION_ROTATE270:
		memcpy(g_orientationTransform3D, Rotation90, sizeof(g_orientationTransform3D));
		break;

	default:
	{
		printf("Error on handling rotation.");
		return;
	}
	}

	g_swapChain->lpVtbl->SetRotation(g_swapChain, displayRotation);

	DXGI_MATRIX_3X2_F inverseScale = { 0 };
	inverseScale._11 = 1.0f / g_compositionScaleX;
	inverseScale._22 = 1.0f / g_compositionScaleY;
	IDXGISwapChain2 *spSwapChain2 = NULL;
	g_swapChain->lpVtbl->QueryInterface(g_swapChain, &IID_IDXGISwapChain2, &spSwapChain2);

	spSwapChain2->lpVtbl->SetMatrixTransform(spSwapChain2, &inverseScale);
	spSwapChain2->lpVtbl->Release(spSwapChain2);

	ID3D11Texture2D *backBuffer = NULL;
	g_swapChain->lpVtbl->GetBuffer(g_swapChain, 0, &IID_ID3D11Texture2D, &backBuffer);

	g_d3dDevice->lpVtbl->CreateRenderTargetView(
		g_d3dDevice, (ID3D11Resource *)backBuffer, NULL, &g_d3dRenderTargetView);

	backBuffer->lpVtbl->Release(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc =
	{
		lround(g_d3dRenderTargetSizeWidth),
		lround(g_d3dRenderTargetSizeHeight),
		1,1,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		{1,0},D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,0,0
	};

	ID3D11Texture2D *depthStencil = NULL;
	g_d3dDevice->lpVtbl->CreateTexture2D(
		g_d3dDevice,
		&depthStencilDesc,
		NULL,
		&depthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc =
	{
		DXGI_FORMAT_D32_FLOAT,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0,
	};
	g_d3dDevice->lpVtbl->CreateDepthStencilView(
		g_d3dDevice,
		(ID3D11Resource *)depthStencil,
		&depthStencilViewDesc,
		&g_d3dDepthStencilView);

	depthStencil->lpVtbl->Release(depthStencil);

	g_screenViewport.TopLeftX = 0.0f;
	g_screenViewport.TopLeftY = 0.0f;
	g_screenViewport.Width = g_d3dRenderTargetSizeWidth;
	g_screenViewport.Height = g_d3dRenderTargetSizeHeight;
	g_screenViewport.MinDepth = 0.0f;
	g_screenViewport.MaxDepth = 1.0f;

	g_d3dContext->lpVtbl->RSSetViewports(g_d3dContext, 1, &g_screenViewport);

	if (CreateWindowSizeDependentResourcesFn)
		CreateWindowSizeDependentResourcesFn();
}

void SetLogicalSize_Base(float logicalSizeWidth, float logicalSizeHeight)
{
	if (g_logicalSizeWidth != logicalSizeWidth ||
		g_logicalSizeHeight != logicalSizeHeight)
	{
		g_logicalSizeWidth = logicalSizeWidth;
		g_logicalSizeHeight = logicalSizeHeight;
		CreateWindowSizeDependentResources_Base();
	}
}

void SetDpi_Base(float dpi)
{
	if (dpi != g_dpi)
	{
		g_dpi = dpi;
		CreateWindowSizeDependentResources_Base();
	}
}

void SetCurrentOrientation_Base(int currentOrientation)
{
	if (g_currentOrientation != currentOrientation)
	{
		g_currentOrientation = (enum Orientations)currentOrientation;
		CreateWindowSizeDependentResources_Base();
	}
}

void SetCompositionScale_Base(float compositionScaleX, float compositionScaleY)
{
	if (g_compositionScaleX != compositionScaleX ||
		g_compositionScaleY != compositionScaleY)
	{
		g_compositionScaleX = compositionScaleX;
		g_compositionScaleY = compositionScaleY;
		CreateWindowSizeDependentResources_Base();
	}
}

void ValidateDevice_Base()
{
	IDXGIDevice3 *dxgiDevice = NULL;
	g_d3dDevice->lpVtbl->QueryInterface(g_d3dDevice, &IID_IDXGIDevice3, &dxgiDevice);

	IDXGIAdapter *deviceAdapter = NULL;
	dxgiDevice->lpVtbl->GetAdapter(dxgiDevice, &deviceAdapter);
	dxgiDevice->lpVtbl->Release(dxgiDevice);

	IDXGIFactory2 *deviceFactory = NULL;
	deviceAdapter->lpVtbl->GetParent(deviceAdapter, &IID_IDXGIFactory2, &deviceFactory);
	deviceAdapter->lpVtbl->Release(deviceAdapter);

	IDXGIAdapter1 *previousDefaultAdapter = NULL;
	deviceFactory->lpVtbl->EnumAdapters1(deviceFactory, 0, &previousDefaultAdapter);
	deviceFactory->lpVtbl->Release(deviceFactory);

	DXGI_ADAPTER_DESC previousDesc;
	previousDefaultAdapter->lpVtbl->GetDesc(previousDefaultAdapter, &previousDesc);
	previousDefaultAdapter->lpVtbl->Release(previousDefaultAdapter);

	IDXGIFactory2 *currentFactory = NULL;
	CreateDXGIFactory1(&IID_IDXGIFactory2, &currentFactory);

	IDXGIAdapter1 *currentDefaultAdapter = NULL;
	currentFactory->lpVtbl->EnumAdapters1(currentFactory, 0, &currentDefaultAdapter);
	currentFactory->lpVtbl->Release(currentFactory);

	DXGI_ADAPTER_DESC currentDesc;
	currentDefaultAdapter->lpVtbl->GetDesc(currentDefaultAdapter, &currentDesc);
	currentDefaultAdapter->lpVtbl->Release(currentDefaultAdapter);

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(g_d3dDevice->lpVtbl->GetDeviceRemovedReason(g_d3dDevice)))
	{
		dxgiDevice = NULL;
		deviceAdapter = NULL;
		deviceFactory = NULL;
		previousDefaultAdapter = NULL;

		HandleDeviceLost_Base();
	}
}

void HandleDeviceLost_Base()
{
	g_swapChain = NULL;

	if (onDeviceLostFn)
		onDeviceLostFn();

	CreateDeviceResources_Base();
	CreateWindowSizeDependentResources_Base();

	if (onDeviceRestoredFn)
		onDeviceRestoredFn();
}

void Present_Base()
{
	HRESULT hr = g_swapChain->lpVtbl->Present(g_swapChain, 1, 0);

	g_d3dContext->lpVtbl->DiscardView(g_d3dContext, (ID3D11View *)g_d3dRenderTargetView);
	g_d3dContext->lpVtbl->DiscardView(g_d3dContext, (ID3D11View *)g_d3dDepthStencilView);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		HandleDeviceLost_Base();
}

DXGI_MODE_ROTATION ComputeDisplayRotation_Base()
{
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

	switch (g_nativeOrientation)
	{
	case DisplayOrientations_Landscape:
	{
		switch (g_currentOrientation)
		{
		case DisplayOrientations_Landscape:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations_Portrait:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations_LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;

		case DisplayOrientations_PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;
		}
		break;
	}

	case DisplayOrientations_Portrait:
	{
		switch (g_currentOrientation)
		{
		case DisplayOrientations_Landscape:
			rotation = DXGI_MODE_ROTATION_ROTATE90;
			break;

		case DisplayOrientations_Portrait:
			rotation = DXGI_MODE_ROTATION_IDENTITY;
			break;

		case DisplayOrientations_LandscapeFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE270;
			break;

		case DisplayOrientations_PortraitFlipped:
			rotation = DXGI_MODE_ROTATION_ROTATE180;
			break;
		}
		break;
	}
	}
	return rotation;
}

void *GetD3D11Device2_Base()
{
	return (void *)g_d3dDevice;
}

void *GetD3D11SwapChain1_Base()
{
	return (void *)g_swapChain;
}

float *GetOrientationTransform_Base()
{
	return g_orientationTransform3D;
}