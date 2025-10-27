#pragma once
#include "structs.h"

class Overlay {
private:
	window wnd;
	D3DX11 d3d11Manager;
	HWND targetWindow;
public:
	Overlay(HWND targetHWND) : targetWindow{ targetHWND } {};
	bool initialize(HWND hwnd);
	void render();
	void shutdown();
	window& getWindow() { return wnd; };
	D3DX11& getManager() { return d3d11Manager; };
	HWND getTargetWindow() { return targetWindow; };
	void resize();
	bool needResize = false;
	bool doDraw = false;
};
static void update(Overlay* overlay);