/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.h
// Purpose:     wxGDIObject class: base class for other GDI classes
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDIOBJ_H_
#define _WX_GDIOBJ_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "gdiobj.h"
#endif

#include "wx/object.h"

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
  wxGDIObject() : m_visible(FALSE) { }
  ~wxGDIObject() { }

  bool IsNull() const { return (m_refData == 0); }

  virtual bool GetVisible() { return m_visible; }
  virtual void SetVisible(bool v) { m_visible = v; }

protected:
  bool m_visible; // Can a pointer to this object be safely taken?
                 // - only if created within FindOrCreate...
};

#endif
    // _WX_GDIOBJ_H_
