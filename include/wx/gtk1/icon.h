/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
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

  wxIcon();
  wxIcon( const wxIcon& icon);
  wxIcon( const char **bits, int width=-1, int height=-1 );
  wxIcon( char **bits, int width=-1, int height=-1 );

  wxIcon& operator = (const wxIcon& icon);
  inline bool operator == (const wxIcon& icon) { return m_refData == icon.m_refData; }
  inline bool operator != (const wxIcon& icon) { return m_refData != icon.m_refData; }
};


#endif // __GTKICONH__
