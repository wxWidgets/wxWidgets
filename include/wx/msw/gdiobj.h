/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.h
// Purpose:     wxGDIObject class: base class for other GDI classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDIOBJ_H_
#define _WX_GDIOBJ_H_

#include "wx/object.h"

#ifdef __GNUG__
#pragma interface "gdiobj.h"
#endif

// wxGDIRefData is the reference-counted data part of a GDI object.
// It contains another counter, m_usageCount, which counts the number
// of times this object has been used; e.g. in SetFont, the count
// is incremented. This is different from reference counting,
// where only the constructors, destructors and (un)clone operations
// affect the reference count.
// THIS IS NOW BEING REMOVED AS REDUNDANT AND ERROR-PRONE

class WXDLLEXPORT wxGDIRefData: public wxObjectRefData {
public:
    inline wxGDIRefData(void)
	{
 	}
};

#define M_GDIDATA ((wxGDIRefData *)m_refData)

class WXDLLEXPORT wxGDIObject: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxGDIObject)
 public:
  inline wxGDIObject(void) { m_visible = FALSE; };
  inline ~wxGDIObject(void) {};

  // Creates the resource
  virtual bool RealizeResource(void) { return FALSE; };

  // Frees the resource
  virtual bool FreeResource(bool WXUNUSED(force) = FALSE) { return FALSE; };

  virtual bool IsFree(void) { return FALSE; };

  inline bool IsNull(void) const { return (m_refData == 0); }

  // Returns handle.
  virtual WXHANDLE GetResourceHandle(void) { return 0; }

  virtual bool GetVisible(void) { return m_visible; }
  virtual void SetVisible(bool v) { m_visible = v; }

protected:
  bool m_visible; // Can a pointer to this object be safely taken?
                 // - only if created within FindOrCreate...
};

#endif
    // _WX_GDIOBJ_H_
