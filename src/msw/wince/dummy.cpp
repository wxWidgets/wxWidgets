/////////////////////////////////////////////////////////////////////////////
// Name:        wince/dummy.cpp
// Purpose:     Main/DllMain
// Author:      Marco Cavallini 
// Modified by:
// Created:     08/11/2002
// RCS-ID:      
// Copyright:   (c) KOAN SAS ( www.koansoftware.com )
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////// 

/* A dummy file to include wx.h. If precompiling wx.h, I
 * always start by compiling this and producing the PCH file.
 * Then subsequent source files use the PCH file.
 *
 * This will produce a big PCH file.
 */ 
	
#include "wx/wxprec.h"

	
// Foils optimizations in Visual C++ (see also app.cpp). Without it,
// dummy.obj isn't linked and we get a linker error.

char wxDummyChar = 0;



