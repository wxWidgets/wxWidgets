/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

#include "wx/motif/bmpmotif.h"
#include "wx/icon.h"

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);

public:
    wxStaticBitmap() { }
    virtual ~wxStaticBitmap();

    wxStaticBitmap(wxWindow *parent, wxWindowID id,
        const wxBitmap& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxASCII_STR(wxStaticBitmapNameStr))
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
        const wxBitmap& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetBitmap(const wxBitmap& bitmap);

    virtual bool ProcessCommand(wxCommandEvent& WXUNUSED(event))
    {
        return false;
    }

    wxBitmap GetBitmap() const { return m_messageBitmap; }

    // Implementation
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();

protected:
    void DoSetBitmap();

protected:
    wxBitmap m_messageBitmap;
    wxBitmap m_messageBitmapOriginal;
    wxBitmapCache m_bitmapCache;
};

#endif
// _WX_STATBMP_H_
