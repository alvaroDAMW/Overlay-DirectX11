#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <winternl.h>
#include "Overlay.h"
#include <d3d11.h>
#include "WinLong.h"
#include <wrl/client.h>
#include <thread>
#include <D3DX11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dX11.lib")
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include <dwmapi.h>
static const float clear_color_with_alpha[4] = { 0,0,0,0 };
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef HWND(WINAPI* CreateWindowInBand)(DWORD dwExStyle, ATOM atom, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD band);
enum ZBID
{
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};
inline LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
   
    switch (msg)
    {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        exit(0);
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
using  querySystemInfo = NTSTATUS(__stdcall*)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);

static querySystemInfo systemInfo = (querySystemInfo)(GetProcAddress)((LoadLibraryA)(("Ntdll.dll")),("NtQuerySystemInformation"));
inline DWORD getProcess(const wchar_t* processName)
{
    ULONG pid{};
    ULONG bytes{};
    auto status = systemInfo(SystemProcessInformation, nullptr, bytes, &bytes);
    PSYSTEM_PROCESS_INFORMATION pProcessInfo;
    do
    {
        pProcessInfo = (PSYSTEM_PROCESS_INFORMATION)(malloc(bytes));
        status = systemInfo(SystemProcessInformation, (PVOID)pProcessInfo, bytes, &bytes);
    } while (!NT_SUCCESS(status));

    do
    {
        pProcessInfo = (PSYSTEM_PROCESS_INFORMATION)((BYTE*)pProcessInfo + pProcessInfo->NextEntryOffset);
    } while (wcscmp(processName, pProcessInfo->ImageName.Buffer) != 0 && pProcessInfo->NextEntryOffset != 0);
    if (wcscmp(processName, pProcessInfo->ImageName.Buffer) == 0)
    {
        pid = (ULONG)pProcessInfo->UniqueProcessId;
    }
    pProcessInfo = nullptr;

    return pid;
}
inline HWND getWindowByPid(DWORD pid)
{
    struct arguments {
        DWORD pid;
        HWND  hwnd;
    } args{ pid, nullptr };

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
        {
			auto args = (arguments*)lParam;

            DWORD windowPid;
            GetWindowThreadProcessId(hwnd, &windowPid);
            if (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == nullptr)
            {
                if (windowPid == (DWORD)args->pid)
                {
                    args->hwnd = hwnd;
                    return FALSE;
                }
            }
            
            return TRUE;
		}, (LPARAM)&args);
    
	return args.hwnd;
}
