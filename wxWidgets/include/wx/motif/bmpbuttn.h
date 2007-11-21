/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTN_H_
#define _WX_BMPBUTTN_H_

#include "wx/motif/bmpmotif.h"

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLEXPORT wxBitmapButton: public wxBitmapButtonBase
{
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
public:
    wxBitmapButton();
    virtual ~wxBitmapButton();
    wxBitmapButton(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxBU_AUTODRAW,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr);

    virtual void SetLabel(const wxBitmap& bitmap)
    {
        SetBitmapLabel(bitmap);
    }
    virtual void SetLabel(const wxString& label)
    {
        wxControl::SetLabel(label);
    }

    virtual void SetBitmapLabel(const wxBitmap& bitmap);

    void SetBitmapSelected(const wxBitmap& sel);
    void SetBitmapFocus(const wxBitmap& focus);
    void SetBitmapDisabled(const wxBitmap& disabled);

    // Implementation
    void DoSetBitmap();
    virtual void ChangeBackgroundColour();
    virtual wxSize DoGetBestSize() const;

protected:
    wxBitmap m_bmpNormalOriginal; // May be different from m_buttonBitmap
    // if m_buttonBitmap has been changed
    // to reflect button background colour
    wxBitmap m_bmpSelectedOriginal;
    wxBitmap m_bmpDisabledOriginal;

    wxBitmapCache m_bitmapCache;

    WXPixmap m_insensPixmap;
};

#endif
// _WX_BMPBUTTN_H_
