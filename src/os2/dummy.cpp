/*
 * File:  dummy.cc
 * Purpose:  See below
 * Author:  Julian Smart
 * Created:  1993
 * Updated:
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* A dummy file to include wx.h. If precompiling wx.h, I
 * always start by compiling this and producing the PCH file.
 * Then subsequent source files use the PCH file.
 *
 * If precompiling wx.h for wxWindows and derived apps,
 * link dummy.obj with your program.
 *
 * This will produce a big PCH file.
 */

#include "wx/wxprec.h"

#define INCL_OS2
#include <os2.h>

#if defined(__VISAGECPP__)
    char wxDummyChar = 0;
#endif

