/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.cpp
// Purpose:     wxGDIObject class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "gdiobj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#endif

#include "wx/gdiobj.h"

IMPLEMENT_DYNAMIC_CLASS(wxGDIObject, wxObject)

/*
void wxGDIObject::IncrementResourceUsage(void)
{
};

void wxGDIObject::DecrementResourceUsage(void)
{
};

*/

