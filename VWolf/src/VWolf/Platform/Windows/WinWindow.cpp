#include "vwpch.h"
#ifdef VW_PLATFORM_WINDOWS
#include "WinWindow.h"

const wchar_t* CLASS_NAME = L"VWOLF_MAIN_WINDOW";

// TODO: Move this in case of needing it.
const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}

namespace VWolf {
    namespace Private {
        LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            WinWindow* pThis;
            if (uMsg == WM_CREATE)
            {
                CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = reinterpret_cast<WinWindow*>(pCreate->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            }
            else
            {
                pThis = (WinWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            }

            if (pThis)
            {
                return pThis->HandleMessage(uMsg, wParam, lParam);
            }
            else
            {
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
            return TRUE;
        }
    }

	WinWindow::WinWindow(void* handle, InitConfiguration config): hInstance(handle)
	{
        this->config = config;
        WNDCLASS wndClass = {};
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpszClassName = CLASS_NAME;
        wndClass.hInstance = (HINSTANCE)hInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.lpfnWndProc = Private::WindowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;

        RegisterClass(&wndClass);
	}

	WinWindow::~WinWindow()
	{
        UnregisterClass(CLASS_NAME, (HINSTANCE)hInstance);
	}

	void WinWindow::Initialize()
	{
        DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX;

        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);

        RECT rect;
        rect.left = (desktop.right / 2) - (config.width / 2);
        rect.top = (desktop.bottom / 2) - (config.height / 2);
        rect.right = rect.left + config.width;
        rect.bottom = rect.top + config.height;

        AdjustWindowRect(&rect, style, false);
        hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            GetWC(config.title),
            style,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            this);
        ShowWindow((HWND)hwnd, SW_SHOW);
        UpdateWindow((HWND)hwnd);        
	}

    void WinWindow::Run() {
        bool running = true;
        while (running) {
            if (!ProcessMessages()) {
                running = false;
            }
            Sleep(10); //<- Why?
        }
    }

    LRESULT WinWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow((HWND)hwnd);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint((HWND)hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_DESKTOP + 1));

            EndPaint((HWND)hwnd, &ps);
        }
        return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc((HWND)hwnd, uMsg, wParam, lParam);
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