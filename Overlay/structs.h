#pragma once
#include "Defs.h"
struct window
{
	HWND hwnd;
	int width;
	int height;
	bool inicialite(const HWND& targetHWND)
	{
		if (!targetHWND)return false;
		targetHWND;
		RECT rect;
		GetWindowRect(targetHWND, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;


		WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, (GetModuleHandleA)(nullptr), nullptr, nullptr, nullptr, nullptr, (L" "), nullptr };
		wc.hInstance = GetModuleHandleA(nullptr);
		wc.hbrBackground = (HBRUSH)(CreateSolidBrush)(RGB(0, 0, 0));
		auto res = RegisterClassExW(&wc);
		#ifdef _DEBUG
		hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, L" ",L" ", WS_POPUP, 0, 0, width, height, NULL, NULL,GetModuleHandleW(0), NULL);
		#else
		PrepareForUIAccess();
		const auto user32 = LoadLibraryA("user32.dll");
		const auto pCreateWindowInBand = (CreateWindowInBand)GetProcAddress(user32, "CreateWindowInBand");
		if (!pCreateWindowInBand)return false;
		hwnd = pCreateWindowInBand(WS_EX_TOOLWINDOW, res, L" ", WS_POPUP, 0, 0, width, height, NULL, NULL, wc.hInstance, NULL, ZBID_UIACCESS);
		#endif
		HWND hwndtest = (GetWindow)(targetHWND, GW_HWNDPREV);
		MARGINS marg = { -1 };
		SetWindowLongA(hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE);
		SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
		SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
		DwmExtendFrameIntoClientArea(hwnd, &marg);

		SetWindowPos(hwnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		MoveWindow(hwnd, 0, 0, 0, 0, FALSE);
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		return hwnd != INVALID_HANDLE_VALUE || hwnd != 0;
	};
};

struct D3DX11
{
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContex;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
	bool inicialite(const HWND& hwnd, int x, int y)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = x;
		sd.BufferDesc.Height = y;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 144;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		UINT createDeviceFlags = 0;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pDeviceContex);
		if (res == DXGI_ERROR_UNSUPPORTED) {
			res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pDeviceContex);
		}
		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		pBackBuffer->Release();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(pDevice, pDeviceContex);
		return res == S_OK;
	}
};
