// Overlay.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include "Overlay.h"





int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	DWORD notepadPid = getProcess(L"Notepad.exe");
	HWND targetWindow = getWindowByPid(notepadPid);
    Overlay overlay{ targetWindow };
    if (overlay.initialize(targetWindow))
    {
        overlay.render();
    }
	overlay.shutdown();
}

