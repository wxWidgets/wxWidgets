/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.h
// Purpose:     wxGDIObject class: base class for other GDI classes
// Author:      David Webster
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDIOBJ_H_
#define _WX_GDIOBJ_H_

#include "wx/object.h"

class WXDLLEXPORT wxGDIRefData: public wxObjectRefData
{
public:
    inline wxGDIRefData()
    { }
};

#define M_GDIDATA ((wxGDIRefData *)m_refData)

class WXDLLEXPORT wxGDIObject: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxGDIObject)
 public:
  inline wxGDIObject() { m_visible = FALSE; };
  inline ~wxGDIObject() {};

  // Creates the resource
  virtual bool RealizeResource(void) { return FALSE; };

  // Frees the resource
  virtual bool FreeResource(bool WXUNUSED(force) = FALSE) { return FALSE; };

  virtual bool IsFree(void) const { return FALSE; };

  inline bool IsNull() const { return (m_refData == 0); }

  virtual bool GetVisible() { return m_visible; }
  virtual void SetVisible(bool v) { m_visible = v; }

protected:
  bool m_visible; // Can a pointer to this object be safely taken?
                 // - only if created within FindOrCreate...
};

#endif
    // _WX_GDIOBJ_H_
