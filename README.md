# DirectX11 Overlay con ImGui — Ejemplo Técnico Avanzado

Este proyecto implementa un **overlay en tiempo real sobre ventanas externas** utilizando **DirectX 11** e **ImGui**, con un sistema de actualización independiente que sincroniza la posición y el tamaño del overlay respecto a la ventana objetivo.

El objetivo principal es **demostrar técnicas avanzadas de renderizado y gestión de ventanas en Win32**, sin usar frameworks externos de alto nivel.  
Está diseñado con un enfoque educativo y técnico, **no invasivo**, para ilustrar conceptos de *real-time rendering*, *window composition* y *sincronización con ventanas externas*.

---

## Características principales

- **Renderizado mediante DirectX 11** con contexto y *swap chain* propio.  
- **Integración con Dear ImGui** (backend Win32 + DX11).  
- **Overlay transparente y no interactivo** que se superpone sobre una ventana externa sin bloquearla (clic-through).  
- **Actualización dinámica de posición y tamaño** al moverse o redimensionarse la ventana objetivo.  
- **Hilo de sincronización dedicado** que controla visibilidad, foco y atributos de la ventana overlay.  
- **Hotkey (F2)** para activar o desactivar el renderizado del overlay sin cerrar la aplicación.  
- **Recreación automática de buffers** al detectar cambios de tamaño.

---

## Arquitectura

La clase principal `Overlay` encapsula tres componentes:

- `Window` — Gestión de la ventana overlay (creación, dimensiones, atributos `WS_EX_*`).  
- `D3D11Manager` — Inicialización y mantenimiento de objetos DirectX (`Device`, `SwapChain`, `RenderTargetView`).  
- `update()` — Hilo auxiliar que sincroniza posición/tamaño y controla `doDraw`.

### Flujo general

1. **Inicialización**: crear ventana overlay, configurar estilos extendidos, inicializar D3D11, lanzar hilo `update`.  
2. **Render loop**: procesar mensajes, `NewFrame` ImGui, dibujar (si la ventana objetivo tiene foco), presentar frame.  
3. **Sincronización**: el hilo `update` monitoriza la ventana objetivo con `GetWindowRect`, llama a `MoveWindow`/`SetWindowPos` y marca `needResize` para recrear buffers cuando haga falta.

---

## Detección de la ventana objetivo (ejemplo incluido)

En este ejemplo el `wWinMain` obtiene el PID del proceso objetivo (Notepad) y recupera su `HWND` con las funciones auxiliares `getProcess` y `getWindowByPid`. A partir de ahí se instancia el `Overlay` pasando el `HWND` objetivo:

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
