/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/statbmp.h
// Purpose:     wxStaticBitmap class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_STATBMP_H_
#define _WX_UNIV_STATBMP_H_

#ifdef __GNUG__
    #pragma interface "univstatbmp.h"
#endif

#include "wx/bitmap.h"

// ----------------------------------------------------------------------------
// wxStaticBitmap
// ----------------------------------------------------------------------------

class wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap()
    {
    }

    wxStaticBitmap(wxWindow *parent,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0)
    {
        Create(parent, -1, label, pos, size, style);
    }

    wxStaticBitmap(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxStaticBitmapNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBitmapNameStr);

    virtual void SetBitmap(const wxBitmap& bitmap);
    virtual void SetIcon(const wxIcon& icon);
    virtual wxBitmap GetBitmap() const { return m_bitmap; }

    wxIcon GetIcon() const;

private:
    virtual wxSize DoGetBestSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);

    // the bitmap which we show
    wxBitmap m_bitmap;

    DECLARE_DYNAMIC_CLASS(wxStaticBitmap)
};

#endif // _WX_UNIV_STATBMP_H_
