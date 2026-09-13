#include <windows.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcA(hwnd, uMsg, wParam, lParam);

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//creating the window variable and initializing it
	WNDCLASSA wc = {};

	//giving the window a name but this is not the title
	wc.lpszClassName = "My_First_Game";

	// Telling Windows which program instance owns this window class
	wc.hInstance = hInstance;

	// Telling Windows how to handle messages sent to our window
	wc.lpfnWndProc = WndProc;

	//now we register the address of the variable 'wc'
	RegisterClassA(&wc);


	//now lets create the window
	HWND hwnd = CreateWindowExA(0,
		"My_First_Game",
		"My 2D Game",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1280,
		720,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	//now show the window you just created
	ShowWindow(hwnd, nCmdShow);

	//device and device context creation code
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* devicecontext = nullptr;

	D3D_FEATURE_LEVEL featureLevel;


	//created the device.
	HRESULT result = D3D11CreateDevice(
	nullptr,
	D3D_DRIVER_TYPE_HARDWARE,
	nullptr,
	0,
	nullptr,
	0,
	D3D11_SDK_VERSION,
	&device,
	&featureLevel,
	&devicecontext
	);

	if (FAILED(result)) {
		MessageBoxA(
			nullptr,
			"Failed to create Direct3D device",
			"Error",
			MB_OK);

		return 0;
	}


	//now we go to the swap chain which now puts the GPU image buffer onto the window dictated by the hwnd
	// the line below is the properties of the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	//how big should the back buffer be since our window was a resolution of 1280 by 720?
	swapChainDesc.BufferDesc.Width = 1280;
	swapChainDesc.BufferDesc.Height = 720;

	//then the pixel format of the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //the image will use 8 bits for red, green, blue and alpha


	//how many buffers do we want?
	swapChainDesc.BufferCount = 1;

	//usage of the back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//now we tell it about the window we want it to render to
	swapChainDesc.OutputWindow = hwnd;


	//This is the technique used in multisampling like anti-aliasing. We are not using multisampling so we set it to 1 and 0
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = TRUE;



	//we create the swapchain now
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Resource* backBuffer = nullptr;

	//getting the legacy DXGI from the D3D11
	IDXGIDevice* dxgiDevice = nullptr;
	result = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));

	if (FAILED(result)) {
		MessageBoxA(
			nullptr,
			"Failed to create the dxgi Device",
			"Error",
			MB_OK);
		return 0;
	}

	//let's get the graphics adapter from the dxgiDevice. 
	//The adapter is basically the target GPU we are gonna use
	IDXGIAdapter* adapter = nullptr;
	result = dxgiDevice->GetAdapter(&adapter);

	if (FAILED(result)) {
		MessageBoxA(
			nullptr,
			"Failed to create the adapter",
			"ERROR",
			MB_OK);
		return 0;
	}
	//now the factory
	IDXGIFactory* factory = nullptr;
	result = adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
	if (FAILED(result)) {
		MessageBoxA(
		nullptr,
		"failed to create the factory",
		"Error",
		MB_OK
		);
	}
	//crate the swap chain now
	result = factory->CreateSwapChain(
	device,
	&swapChainDesc,
	&swapChain);

	if (FAILED(result)) {
		MessageBoxA(
		nullptr,
		"Failed to create swap chain",
		"error",
		MB_OK);
	}

	//now lets ask swap chain for the buffer
	result = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Resource),
		reinterpret_cast<void**>(&backBuffer)
	);
	

	//creating the render target view
	ID3D11RenderTargetView* renderTargetView = nullptr;
	result = device->CreateRenderTargetView(
	backBuffer,
	nullptr,
	&renderTargetView
	);

	if (FAILED(result)) {
		MessageBoxA(
		nullptr,
		"Failed to create a render target view",
		"ERROR",
		MB_OK
		);
		return 0;
	}

	//now let's make the Output Merger Render targets - this is the final part of the graphics pipeline 
	devicecontext->OMSetRenderTargets(
	1, //this is binding one render target
	&renderTargetView, //the reference for the render target being binded to
	nullptr
	);

	

	MSG msg = {};
	bool running = true;

	float clearColor[4]{
		0.0f, //red
		0.0f, //green
		0.0f, //blue
		1.0f  //alpha
	};
	
	while (running) {

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				running = false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Update
		//Game logic appears here too
		//Render
		
		devicecontext->ClearRenderTargetView(
			renderTargetView,
			clearColor // the alpha being full makes it black
		);

		swapChain->Present(1, 0);

	}

	return 0;
}