/////////////////////////////////////////////////////////////////////////////
// Name:        wince/main.cpp
// Purpose:     Main/DllMain
// Author:      Marco Cavallini 
// Modified by:
// Created:     08/11/2002
// RCS-ID:      
// Copyright:   (c) KOAN SAS ( www.koansoftware.com )
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////// 


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

//#include "wx/event.h"
//#include "wx/app.h"

//#include "wx/msw/private.h"

// from src/msw/app.cpp
//extern void WXDLLEXPORT wxEntryCleanup();

// ----------------------------------------------------------------------------

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

// ----------------------------------------------------------------------------
// global 
// ----------------------------------------------------------------------------

HINSTANCE wxhInstance = 0;

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

HINSTANCE wxGetInstance()
{
    return wxhInstance;
}

void wxSetInstance(HINSTANCE hInst)
{
    wxhInstance = hInst;
}
