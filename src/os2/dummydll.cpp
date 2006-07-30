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

#define INCL_OS2
#include <os2.h>

#if defined(__VISAGECPP__)
    char wxDummyChar=0;
#endif

