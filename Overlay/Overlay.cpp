#include "Overlay.h"

bool Overlay::initialize(HWND hwnd)
{
	if(!wnd.inicialite(hwnd))
	{
		printf("[-] Failed to initialize window struct\n");
		return false;
	}
	if (!d3d11Manager.inicialite(wnd.hwnd,wnd.width,wnd.height))
	{
		printf("[-] Failed to initialize D3D11 struct\n");
		return false;
	}

	std::thread(&update,this).detach();

	return true;
}

void Overlay::render()
{
	while (true)
	{
		MSG msg;
		if (::PeekMessageA(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		if (GetForegroundWindow() == targetWindow || (GetForegroundWindow)() == wnd.hwnd)
		{
			if (doDraw)
				ImGui::ShowDemoWindow();
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(10, 10), ImColor(255, 0, 0), "Overlay Initialized!");
		}
		else {
			Sleep(10);
		}
		ImGui::Render();
		d3d11Manager.pDeviceContex->OMSetRenderTargets(1, &d3d11Manager.pRenderTargetView, nullptr);
		d3d11Manager.pDeviceContex->ClearRenderTargetView(d3d11Manager.pRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		if (needResize)
		{
			resize();
			needResize = false;
		}
		d3d11Manager.pSwapChain->Present(1, 0);
		
	}
}

void Overlay::shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ZeroMemory(&wnd, sizeof(wnd));
	ZeroMemory(&d3d11Manager, sizeof(d3d11Manager));
}

void Overlay::resize()
{
	if (d3d11Manager.pRenderTargetView) { d3d11Manager.pRenderTargetView->Release(); d3d11Manager.pRenderTargetView = nullptr; }
	d3d11Manager.pSwapChain->ResizeBuffers(0, wnd.width, wnd.height, DXGI_FORMAT_UNKNOWN, 0);
	ID3D11Texture2D* pBackBuffer;
	d3d11Manager.pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	d3d11Manager.pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &d3d11Manager.pRenderTargetView);
	pBackBuffer->Release();
}

void update(Overlay* overlay)
{
    while (true)
    {
		HWND hwnd = overlay->getWindow().hwnd;
		HWND targetWindow = overlay->getTargetWindow();
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			if ((GetForegroundWindow)() == targetWindow || (GetForegroundWindow)() == hwnd) {
				overlay->doDraw = !overlay->doDraw;
				LONG wndAtribb = WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE;
				if (overlay->doDraw)
				{
					wndAtribb = WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
					(SetWindowLongA)(overlay->getWindow().hwnd, GWL_EXSTYLE, wndAtribb);
					(SetForegroundWindow)(overlay->getWindow().hwnd);
				}
				else
				{
					(SetWindowLongA)(overlay->getWindow().hwnd, GWL_EXSTYLE, wndAtribb);
					(SetForegroundWindow)(overlay->getTargetWindow());
				}
			}
		}

        if (targetWindow != nullptr)
        {
            RECT targetrect;
            RECT mainRect;
            (GetWindowRect)(overlay->getWindow().hwnd, &mainRect);
            (GetWindowRect)(targetWindow, &targetrect);
            if ((GetForegroundWindow)() == targetWindow)
            {
                HWND hwndtest = (GetWindow)((GetForegroundWindow)(), GW_HWNDPREV);
                SetWindowPos(overlay->getWindow().hwnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            if (targetrect.top != mainRect.top || targetrect.left != mainRect.left || targetrect.bottom != mainRect.bottom || targetrect.right != mainRect.right)
            {
                (MoveWindow)(overlay->getWindow().hwnd, targetrect.left, targetrect.top, targetrect.right - targetrect.left, targetrect.bottom - targetrect.top, false);
                SetWindowPos(overlay->getWindow().hwnd, 0, targetrect.top, targetrect.left, targetrect.right - targetrect.left, targetrect.bottom - targetrect.top, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				overlay->getWindow().width = targetrect.right - targetrect.left;
				overlay->getWindow().height = targetrect.bottom - targetrect.top;
				overlay->needResize = true;
            }
        }
        (Sleep)(5);
    }
}
