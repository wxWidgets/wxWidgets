/*
 * File:	dummy.cc
 * Purpose:	See below
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)dummy.cc	1.2 5/9/94"; */

/* A dummy file to include wx.h. If precompiling wx.h, I
 * always start by compiling this and producing the PCH file.
 * Then subsequent source files use the PCH file.
 *
 * If precompiling wx.h for wxWindows and derived apps,
 * link dummy.obj with your program (the MSC 7 linker complains otherwise).
 *
 * This is the only convenient way I found to use precompiled headers
 * under MSC 7.
 *
 * This will produce a big PCH file.
 */



#if defined(__BORLANDC__)
#if !(defined(__WIN32__) || defined(__NT__) || defined(__WIN32__))
#pragma hdrfile "c:\wx\src\msw\wx.pch"
#endif

#pragma hdrstart
#endif

#include "wx/wxprec.h"
#include "windows.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// Foils optimizations in Visual C++ (see also wx_main.cc)
char wxDummyChar=0;

#if defined(WXUSINGDLL)

// NT defines APIENTRY, 3.x not
#if !defined(APIENTRY)
#define APIENTRY FAR PASCAL
#endif
 
#ifdef __WATCOMC__
int PASCAL
#else
int APIENTRY
#endif

 WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR m_lpCmdLine,
                    int nCmdShow )
{
  return wxEntry((WXHINSTANCE) hInstance, (WXHINSTANCE) hPrevInstance, m_lpCmdLine, nCmdShow);
}
#endif


