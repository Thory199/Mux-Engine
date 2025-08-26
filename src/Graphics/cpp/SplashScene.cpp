#include "rtx_to_bitmap.h"
#include "SplashScene.h"
#include <windows.h>
#include <string>

static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void mostrarSplashScreen(const char* rutaImagen, int ancho, int alto, int milisegundos) {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    const wchar_t CLASS_NAME[] = L"SplashScreenClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = SplashWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Convertir ruta a formato wide string
    int len = MultiByteToWideChar(CP_UTF8, 0, rutaImagen, -1, NULL, 0);
    std::wstring rutaW(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, rutaImagen, -1, &rutaW[0], len);


    // Centrar la ventana en pantalla
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - ancho) / 2;
    int y = (screenHeight - alto) / 2;

    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"",
        WS_POPUP,
        x, y, ancho, alto,
        NULL, NULL, hInstance, NULL
    );

    // Cargar imagen BMP
    HBITMAP hBitmap = cargarRTXComoBitmap("Data/Interface/splash.rtx");
    if (!hBitmap) return;

    ShowWindow(hwnd, SW_SHOW);
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, ancho, alto, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

    UpdateWindow(hwnd);

    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HGDIOBJ oldBmp = SelectObject(hdcMem, hBitmap);
    BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, oldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);

    // Esperar el tiempo deseado
    Sleep(milisegundos);

    // Cerrar splash
    DestroyWindow(hwnd);
    DeleteObject(hBitmap);
    UnregisterClass(CLASS_NAME, hInstance);
}
