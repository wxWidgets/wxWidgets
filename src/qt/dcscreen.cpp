/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

#include "wx/dcscreen.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC,wxPaintDC)

wxScreenDC::wxScreenDC(void)
{
  m_ok = FALSE;
};

wxScreenDC::~wxScreenDC(void)
{
  EndDrawingOnTop();
};

bool wxScreenDC::StartDrawingOnTop( wxWindow *WXUNUSED(window) )
{
  return TRUE;
};

bool wxScreenDC::StartDrawingOnTop( wxRectangle *WXUNUSED(rect) )
{
  return TRUE;
};

bool wxScreenDC::EndDrawingOnTop(void)
{
  return TRUE;
};
