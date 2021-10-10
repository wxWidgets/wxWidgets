/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTN_H_
#define _WX_BMPBUTTN_H_

#include "wx/motif/bmpmotif.h"

#define wxDEFAULT_BUTTON_MARGIN 4

class WXDLLIMPEXP_CORE wxBitmapButton: public wxBitmapButtonBase
{
public:
    wxBitmapButton();
    virtual ~wxBitmapButton();
    wxBitmapButton(wxWindow *parent, wxWindowID id,
        const wxBitmapBundle& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxButtonNameStr))
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
        const wxBitmapBundle& bitmap,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool CreateCloseButton(wxWindow* parent,
                           wxWindowID winid,
                           const wxString& name = wxString());
    // Implementation
    virtual void ChangeBackgroundColour();

protected:
    virtual wxSize DoGetBestSize() const;

    virtual void DoSetBitmap(const wxBitmapBundle& bitmap, State which);
    virtual void OnSetBitmap();

    // original bitmaps may be different from the ones we were initialized with
    // if they were changed to reflect button background colour
    wxBitmapBundle m_bitmapsOriginal[State_Max];

    wxBitmapCache m_bitmapCache;

    WXPixmap m_insensPixmap;

    wxDECLARE_DYNAMIC_CLASS(wxBitmapButton);
};

#endif // _WX_BMPBUTTN_H_
