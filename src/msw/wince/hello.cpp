// -----------------------------------------------------------
// This hello program will test wxBase for WinCE
// -----------------------------------------------------------

// Include WinCE headers
#include <windows.h>
#include <windowsx.h>
#include "resource.h"

// Include wxWindows headers
#include "wx/app.h"
#include "wx/string.h"

// Do not undef DrawText, since we need it here
#include "wx/msw/private.h"

// -----------------------------------------------------------
// Hello world code
// -----------------------------------------------------------

HINSTANCE hInst = NULL;  // Local copy of hInstance
HWND hwndMain = NULL;    // Handle to Main window returned from CreateWindow

TCHAR szAppName[] = TEXT("Hello Pocket PC Application");
TCHAR szTitle[]   = TEXT("Hello Pocket PC");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    LRESULT     lResult = TRUE;
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch(msg)
    {
        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wp,lp))
            {
            case IDOK:
                SendMessage(hwnd,WM_CLOSE,0,0);
                break;
            default:
                return DefWindowProc(hwnd, msg, wp, lp);
            }
            break;
        case WM_PAINT:
        {
            hdc = BeginPaint (hwnd, &ps);
            GetClientRect (hwnd, &rect);

            // Test wxString
            wxString test( wxT("Hello wxWindows!") );
            DrawText (hdc, test.c_str(), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            EndPaint (hwnd, &ps);
        }       
        break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            lResult = DefWindowProc(hwnd, msg, wp, lp);
        break;
    }
    return (lResult);
}


BOOL InitInstance (HINSTANCE hInstance, int CmdShow )
{

    hInst = hInstance;
    hwndMain = CreateWindow(szAppName,                      
                    szTitle,
                    WS_VISIBLE,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    NULL, NULL, hInstance, NULL );

    if ( !hwndMain )        
    {
        return FALSE;
    }
    ShowWindow(hwndMain, CmdShow );
    UpdateWindow(hwndMain);
    return TRUE;
}

BOOL InitApplication ( HINSTANCE hInstance )
{
    WNDCLASS wc;
    BOOL f;

    wc.style = CS_HREDRAW | CS_VREDRAW ;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.hInstance = hInstance;
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;
    
    f = (RegisterClass(&wc));

    return f;
}


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPWSTR     lpCmdLine,
                   int        CmdShow)

{
    // We use wxBase as a first test
    wxInitialize();

    MSG msg;
    HWND hHelloWnd = NULL;  
    HACCEL hAccel = NULL;
    
    //Check if Hello.exe is running. If it's running then focus on the window
    hHelloWnd = FindWindow(szAppName, szTitle); 
    if (hHelloWnd) 
    {
        SetForegroundWindow (hHelloWnd);    
        return 0;
    }

    if ( !hPrevInstance )
    {
        if ( !InitApplication ( hInstance ) )
        { 
            return (FALSE); 
        }

    }
    if ( !InitInstance( hInstance, CmdShow )  )
    {
        return (FALSE);
    }
    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    while ( GetMessage( &msg, NULL, 0,0 ) == TRUE )
    {
        if (!TranslateAccelerator(hwndMain,hAccel, &msg)) 
        {
            TranslateMessage (&msg);
            DispatchMessage(&msg);
        }
    }

    // Quit wxBase
    wxUninitialize();

    return (msg.wParam);
}


