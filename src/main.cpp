#include <windows.h>
#include <memoryapi.h>

#include "utils_type.h"

struct win32_offscreen_bufffer 
{
    BITMAPINFO BitmapInfo;
    void* BitmapMemory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

global_variable bool g_running = false;
global_variable win32_offscreen_bufffer g_backbuffer;
internal void DrawGradient(win32_offscreen_bufffer* buffer, int xOffset, int yOffset)
{   
    uint8* row = (uint8*)buffer->BitmapMemory;
    for (int y = 0; y < buffer->Height; ++y)
    {
        uint32* pixel = (uint32*)row;
        for (int x = 0; x<  buffer->Width; ++x)
        {
            // RR GG BB XX
            // LITTLE ENDIAN 
            // In Menory :  BB GG RR xx
            // In Register : xx RR GG BB
            // Pixel (32bit)
            // *pixel = (uint8)(i+xOffset);
            // ++pixel;


            // //GREEN
            // *pixel = (uint8)(j+yOffset);
            // ++pixel;

            // // RED
            // *pixel = (uint8)0;
            // ++pixel;

            // *pixel = (uint8)0;
            uint8 Blue= (x + xOffset);
            uint8 Green = (y + yOffset);
            *pixel++ = ((Green << 8) | Blue);
        }
        row += buffer->Pitch;
    }
}

internal void Win32_ResizeDIBSection(win32_offscreen_bufffer* buffer,int width,int height) // Device independent bitmap
{
    if( buffer->BitmapMemory )
    {
        VirtualFree( buffer->BitmapMemory ,0,MEM_RELEASE);
    }
    buffer->Width = width;
    buffer->Height = height;
    buffer->BitmapInfo.bmiHeader.biSize = sizeof(buffer->BitmapInfo.bmiHeader);
    buffer->BitmapInfo.bmiHeader.biWidth = width;
    buffer->BitmapInfo.bmiHeader.biHeight = -height;
    buffer->BitmapInfo.bmiHeader.biPlanes = 1;
    buffer->BitmapInfo.bmiHeader.biBitCount = 32;
    buffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;
    buffer->BytesPerPixel = 4;
    int bitmapMemorySize = (width * height) *buffer->BytesPerPixel;
    buffer->BitmapMemory = VirtualAlloc(0,bitmapMemorySize,MEM_COMMIT,PAGE_READWRITE);
    buffer->Pitch = buffer->BytesPerPixel * width;
}

internal void Win32_UpdateCopyBufferToWindow(HDC deviceContext,RECT windowRect,win32_offscreen_bufffer* buffer,int x , int y,int width,int height)
{
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    StretchDIBits(deviceContext,0,0,buffer->Width,buffer->Height,
                                0,0,windowWidth,windowHeight,
                               buffer->BitmapMemory,&buffer->BitmapInfo,DIB_RGB_COLORS,SRCCOPY);
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
        Win32_ResizeDIBSection(&g_backbuffer,width,height);
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
        RECT clientRect;
        GetClientRect(window,&clientRect);
        Win32_UpdateCopyBufferToWindow(deviceContext,clientRect,&g_backbuffer,x,y,width,height);
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
            int xoffset;
            int yOffset;
            while(g_running)
            {
               
                while(PeekMessage(&message,0,0,0,PM_REMOVE))
                {
                    if(message.message == WM_QUIT)
                    {
                        g_running = false;
                    }
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                {
                    DrawGradient(&g_backbuffer,xoffset,yOffset);
                    HDC deviceContext = GetDC(windowHandle);
                    RECT clientRect;
                    GetClientRect(windowHandle,&clientRect);
                    int windowWidth = clientRect.right - clientRect.left;
                    int windowHeight = clientRect.bottom - clientRect.top;
                    Win32_UpdateCopyBufferToWindow(deviceContext,clientRect,&g_backbuffer,0,0,windowWidth,windowHeight);
                    ReleaseDC(windowHandle,deviceContext);
                }
                xoffset++;
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