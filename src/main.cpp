#include "windows.h"

LRESULT CALLBACK MainWindowCallBack(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message)
    {
    case WM_CREATE:
    {
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
        OutputDebugStringA("WM_DESTROY\n");
    }
    break;
    case WM_CLOSE:
    {
        OutputDebugStringA("WM_CLOSE\n");
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
            MSG message;
            for(;;)
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