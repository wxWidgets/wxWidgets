/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKICONH__
#define __GTKICONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxIcon;

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

class wxIcon: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxIcon)

public:

  wxIcon(void) {};

  inline wxIcon(const wxIcon& icon) { Ref(icon); }
  inline wxIcon(const wxIcon* icon) { if (icon) Ref(*icon); }

  wxIcon( char **bits, int width=-1, int height=-1 ) :
    wxBitmap( bits ) {};
    
  inline wxIcon& operator = (const wxIcon& icon) { if (*this == icon) return (*this); Ref(icon); return *this; }
  inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
  inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }
};


#endif // __GTKICONH__
