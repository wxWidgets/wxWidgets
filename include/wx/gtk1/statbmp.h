/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/statbmp.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICBITMAPH__
#define __GTKSTATICBITMAPH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#include "wx/object.h"
#include "wx/control.h"
#include "wx/bitmap.h"
#include "wx/icon.h"

//-----------------------------------------------------------------------------
// wxStaticBitmap
//-----------------------------------------------------------------------------

class wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap();
    wxStaticBitmap( wxWindow *parent,
                    wxWindowID id,
                    const wxBitmap& label,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxStaticBitmapNameStr );
    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxBitmap& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxStaticBitmapNameStr);

    virtual void SetBitmap( const wxBitmap& bitmap );
    virtual void etIcon(const wxIcon& icon) { SetBitmap( icon ); }

    wxBitmap& GetBitmap() { return m_bitmap; }
    const wxBitmap& GetBitmap() const { return m_bitmap; }

    // for compatibility with wxMSW
    const wxIcon& GetIcon() const
    {
        // don't use wxDynamicCast, icons and bitmaps are really the same thing
        // in wxGTK
        return (const wxIcon &)m_bitmap;
    }

private:
    // creates the new pixmap widget
    void CreatePixmapWidget();

    wxBitmap   m_bitmap;

    DECLARE_DYNAMIC_CLASS(wxStaticBitmap)
};

#endif // __GTKSTATICBITMAPH__
