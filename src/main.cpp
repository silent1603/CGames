#include <windows.h>
#include <memoryapi.h>
#include "utils_type.h"

global_variable bool g_running = false;
global_variable void* g_bitmapMemory;
global_variable BITMAPINFO g_bitmapInfo;


internal void Win32_ResizeDIBSection(int width,int height) // Device independent bitmap
{
    if(g_bitmapMemory)
    {
        VirtualFree(g_bitmapMemory,0,MEM_RELEASE);
    }
    g_bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
    g_bitmapInfo.bmiHeader.biWidth = width;
    g_bitmapInfo.bmiHeader.biHeight = height;
    g_bitmapInfo.bmiHeader.biPlanes = 1;
    g_bitmapInfo.bmiHeader.biBitCount = 32;
    g_bitmapInfo.bmiHeader.biCompression = BI_RGB;
    int bytesPerPixel = 4;
    int bitmapMemorySize = (width * height) * bytesPerPixel;
    g_bitmapMemory = VirtualAlloc(0,bitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);
}

internal void Win32_UpdateWindow(HDC deviceContext,int x,int y,int width,int height)
{
    StretchDIBits(deviceContext,x,y,width,height,
                                x,y,width,height,
                               g_bitmapMemory,&g_bitmapInfo,DIB_RGB_COLORS,SRCCOPY);
}

LRESULT CALLBACK MainWindowCallBack(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message)
    {
    case WM_CREATE:
    {
        RECT clientRect;
        GetClientRect(window,&clientRect);
        int width = clientRect.right -clientRect.left;
        int height =clientRect.bottom - clientRect.top;
        Win32_ResizeDIBSection(width,height);
        OutputDebugStringA("WM_CREATE\n");
    }
    break;
    case WM_SIZE:
    {
        OutputDebugStringA("WM_SIZE\n");
    }
    break;
    case WM_DESTROY:
    {
        g_running = false;
    }
    break;
    case WM_CLOSE:
    {
        g_running =false;
    }
    break;
    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVEAPP\n");
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT paint;
        HDC deviceContext = BeginPaint(window,&paint);
        int x = paint.rcPaint.left;
        int y = paint.rcPaint.top;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;
        int width = paint.rcPaint.left - paint.rcPaint.right;
        Win32_UpdateWindow(deviceContext,x,y,width,height);
        PatBlt(deviceContext,x,y,width,height,WHITENESS);
        EndPaint(window,&paint);
    }
    break;
    default:
    {
        OutputDebugStringA("default\n");
        result = DefWindowProc(window,message,wParam,lParam);
    }
       
        break;
    }
    return result;
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PreInstance, LPSTR CommandLine, int ShowCode)
{
    WNDCLASS windowClass = {};
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = MainWindowCallBack;
    windowClass.hInstance = Instance;
    windowClass.lpszClassName = "CGamesWindowClass";
    // windowClass.hIcon
    if(RegisterClass(&windowClass))
    {
        HWND windowHandle = CreateWindowEx(0,windowClass.lpszClassName,"CGames",WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,0,0,Instance,0);
        if(windowHandle)
        {
            g_running =true;
            MSG message;
            while(g_running)
            {
                BOOL messageResult = GetMessage(&message,0,0,0);
                if(messageResult > 0)
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
             //TODO(trung) : logging
        }
    }
    else
    {
        //TODO(trung) : logging
    }


    return 0;
}