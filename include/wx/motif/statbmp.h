/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#ifdef __GNUG__
#pragma interface "statbmp.h"
#endif

#include "wx/control.h"
#include "wx/icon.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticBitmapNameStr;

class WXDLLEXPORT wxStaticBitmap : public wxControl
{
DECLARE_DYNAMIC_CLASS(wxStaticBitmap)

public:
    wxStaticBitmap() { }
    ~wxStaticBitmap();

    wxStaticBitmap(wxWindow *parent, wxWindowID id,
            const wxBitmap& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxStaticBitmapNameStr)
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxBitmap& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxStaticBitmapNameStr);

    virtual void SetBitmap(const wxBitmap& bitmap);

    virtual bool ProcessCommand(wxCommandEvent& WXUNUSED(event))
    {
        return FALSE;
    }

    wxBitmap& GetBitmap() const { return (wxBitmap&) m_messageBitmap; }

    // for compatibility with wxMSW
    const wxIcon& GetIcon() const
    {
        // don't use wxDynamicCast, icons and bitmaps are really the same thing
        return (const wxIcon &)m_messageBitmap;
    }

    // for compatibility with wxMSW
    void  SetIcon(const wxIcon& icon)
    {
        SetBitmap( icon );
    }

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return FALSE; }

    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();

protected:
    wxBitmap m_messageBitmap;
};

#endif
    // _WX_STATBMP_H_
