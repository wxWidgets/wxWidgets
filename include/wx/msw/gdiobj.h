/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/gdiobj.h
// Purpose:     wxGDIObject class: base class for other GDI classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDIOBJ_H_
#define _WX_GDIOBJ_H_

#ifdef __GNUG__
    #pragma interface "gdiobj.h"
#endif

#include "wx/object.h"  // base class

// ----------------------------------------------------------------------------
// wxGDIRefData is the base class for wxXXXData structures which contain the
// real data for the GDI object and are shared among all wxWin objects sharing
// the same native GDI object
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIRefData : public wxObjectRefData
{
    // this class is intentionally left blank
};

// ----------------------------------------------------------------------------
// wxGDIObject
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIObject : public wxObject
{
public:
    wxGDIObject() { m_visible = FALSE; };

    // Creates the resource
    virtual bool RealizeResource() { return FALSE; };

    // Frees the resource
    virtual bool FreeResource(bool WXUNUSED(force) = FALSE) { return FALSE; }

    virtual bool IsFree() const { return FALSE; }

    bool IsNull() const { return (m_refData == 0); }

    // Returns handle.
    virtual WXHANDLE GetResourceHandle() { return 0; }

    virtual bool GetVisible() { return m_visible; }
    virtual void SetVisible(bool v) { m_visible = v; }

protected:
    bool m_visible; // TRUE only if we should delete this object ourselves

private:
    DECLARE_DYNAMIC_CLASS(wxGDIObject)
};

#endif
    // _WX_GDIOBJ_H_
