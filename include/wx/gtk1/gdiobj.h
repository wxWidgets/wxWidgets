/////////////////////////////////////////////////////////////////////////////
// Name:        gdiobj.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GDIOBJH__
#define __GDIOBJH__

#include "wx/object.h"

#ifdef __GNUG__
#pragma interface
#endif

class WXDLLEXPORT wxGDIObject: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxGDIObject)
 public:
  inline wxGDIObject(void) { m_visible = FALSE; };
  inline ~wxGDIObject(void) {};

  virtual bool GetVisible(void) { return m_visible; }
  virtual void SetVisible(bool v) { m_visible = v; }

protected:
  bool m_visible; // Can a pointer to this object be safely taken?
                 // - only if created within FindOrCreate...
};

#endif
    // __GDIOBJH__
