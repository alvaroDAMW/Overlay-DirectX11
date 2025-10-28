# DirectX11 Overlay with ImGui — Advanced Technical Example

This project implements a **real-time overlay on external windows** using **DirectX 11** and **ImGui**, with an independent update system that synchronizes the overlay’s position and size relative to the target window.

The main goal is to **demonstrate advanced rendering and window-management techniques on Win32**, without relying on high-level external frameworks.
It is designed with an educational and technical focus, in a **non-intrusive** way, to illustrate concepts of *real-time rendering*, *window composition*, and *synchronization with external windows*.

## Main Features

* **Rendering via DirectX 11** with its own device, context, and swap chain.
* **Dear ImGui integration** (Win32 + DX11 backend).
* **Transparent, non-interactive overlay** layered above an external window without blocking input (click-through).
* **Dynamic position and size updates** when the target window moves or resizes.
* **Dedicated synchronization thread** controlling visibility, focus, and overlay window attributes.
* **Hotkey (F2)** to enable/disable overlay rendering without closing the application.
* **Automatic buffer recreation** on resize events.

## Architecture

The main `Overlay` class encapsulates three components:

* `Window` — Manages the overlay window (creation, dimensions, and extended styles `WS_EX_*`).
* `D3D11Manager` — Initializes and maintains DirectX objects (`ID3D11Device`, `IDXGISwapChain`, `ID3D11RenderTargetView`).
* `update()` — Auxiliary thread that synchronizes position/size and controls the `doDraw` flag.

### General Flow

1. **Initialization**: create the overlay window, configure extended styles, initialize D3D11, and launch the `update` thread.
2. **Render loop**: process messages, start ImGui `NewFrame`, draw (only if the target window has focus), and present the frame.
3. **Synchronization**: the `update` thread monitors the target window with `GetWindowRect`, calls `MoveWindow`/`SetWindowPos`, and flags `needResize` to recreate buffers when needed.

## Target Window Detection (included example)

In this example, `wWinMain` retrieves the target process PID (Notepad) and obtains its `HWND` using helper functions `getProcess` and `getWindowByPid`. Then it instantiates `Overlay` with the target `HWND`:

```cpp
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
```
