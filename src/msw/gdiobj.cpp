/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.cpp
// Purpose:     wxGDIObject class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
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
#include "wx/msw/private.h"
#include "assert.h"

IMPLEMENT_DYNAMIC_CLASS(wxGDIObject, wxObject)

/*
void wxGDIObject::IncrementResourceUsage(void)
{
	if ( !M_GDIDATA )
		return;

//  wxDebugMsg("Object %ld about to be incremented: %d\n", (long)this, m_usageCount);
  M_GDIDATA->m_usageCount ++;
};

void wxGDIObject::DecrementResourceUsage(void)
{
	if ( !M_GDIDATA )
		return;

  M_GDIDATA->m_usageCount --;
  if (wxTheApp)
    wxTheApp->SetPendingCleanup(TRUE);
//  wxDebugMsg("Object %ld decremented: %d\n", (long)this, M_GDIDATA->m_usageCount);
  if (M_GDIDATA->m_usageCount < 0)
  {
    char buf[80];
    sprintf(buf, "Object %ld usage count is %d\n", (long)this, M_GDIDATA->m_usageCount);
    wxDebugMsg(buf);
  }
//  assert(M_GDIDATA->m_usageCount >= 0);
};

*/

