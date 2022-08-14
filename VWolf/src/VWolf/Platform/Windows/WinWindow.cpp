#include "vwpch.h"
#ifdef VW_PLATFORM_WINDOWS

#include <Windows.h>
#include "WinWindow.h"

// TODO: Remove this later
const wchar_t* CLASS_NAME = L"My window";

namespace VWolf {
	WinWindow::WinWindow(void* handle): hInstance(handle)
	{
        WNDCLASS wndClass = {};
        wndClass.lpszClassName = CLASS_NAME;
        wndClass.hInstance = (HINSTANCE)hInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.lpfnWndProc = WinWindow::WindowProc;

        RegisterClass(&wndClass);
	}

	WinWindow::~WinWindow()
	{
        UnregisterClass(CLASS_NAME, (HINSTANCE)hInstance);
	}

	void WinWindow::Initialize()
	{
        DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);

        RECT rect;
        rect.left = (desktop.right / 2) - (width / 2);
        rect.top = (desktop.bottom / 2) - (height / 2);
        rect.right = rect.left + width;
        rect.bottom = rect.top + height;

        AdjustWindowRect(&rect, style, false);
        hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Title",
            style,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            nullptr);
        ShowWindow((HWND)hwnd, SW_SHOW);

        bool running = true;
        while (running) {
            if (!ProcessMessages()) {
                std::cout << "Closing window!" << std::endl;
                running = false;
            }
            Sleep(10); //<- Why?
        }
	}

    LRESULT CALLBACK WinWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_DESKTOP + 1));

            EndPaint(hwnd, &ps);
        }
        return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    bool WinWindow::ProcessMessages() {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }
}

#endif