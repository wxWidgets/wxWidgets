/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.h
// Purpose:     wxGDIObject class: base class for other GDI classes
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDIOBJ_H_
#define _WX_GDIOBJ_H_

#include "wx/object.h"

#ifdef __GNUG__
#pragma interface "gdiobj.h"
#endif

class WXDLLEXPORT wxGDIRefData: public wxObjectRefData {
public:
    inline wxGDIRefData()
	{
 	}
};

#define M_GDIDATA ((wxGDIRefData *)m_refData)

class WXDLLEXPORT wxGDIObject: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxGDIObject)
 public:
  inline wxGDIObject() { m_visible = FALSE; };
  inline ~wxGDIObject() {};

  inline bool IsNull() const { return (m_refData == 0); }

  virtual bool GetVisible() { return m_visible; }
  virtual void SetVisible(bool v) { m_visible = v; }

protected:
  bool m_visible; // Can a pointer to this object be safely taken?
                 // - only if created within FindOrCreate...
};

#endif
    // _WX_GDIOBJ_H_
