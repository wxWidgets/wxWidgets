/*
 * File:  dummydll.cc
 * Purpose:
 * Author:  Julian Smart
 * Created:  1993
 * Updated:
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)dummydll.cc  1.2 5/9/94"; */

/*
 * A dummy file to include wx.h. If precompiling wx.h, I
 * always start by compiling this and producing the PCH file.
 * Then subsequent source files use the PCH file.
 */

#include "wx/wxprec.h"

// Foils optimizations in Visual C++ (see also app.cpp). Without it,
// dummy.obj isn't linked and we get a linker error.
#if defined(__VISUALC__)
    char wxDummyChar=0;
#endif

