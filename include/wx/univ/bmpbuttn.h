/////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/bmpbuttn.h
// Purpose:     wxBitmapButton class for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_BMPBUTTN_H_
#define _WX_UNIV_BMPBUTTN_H_

#ifdef __GNUG__
    #pragma interface "univbmpbuttn.h"
#endif

class WXDLLEXPORT wxBitmapButton : public wxBitmapButtonBase
{
public:
    wxBitmapButton() { }

    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual void SetMargins(int x, int y)
    {
        SetImageMargins(x, y);

        wxBitmapButtonBase::SetMargins(x, y);
    }

    virtual bool Enable(bool enable = TRUE);

    virtual void SetCurrent(bool doit = TRUE);

    virtual void Press();
    virtual void Release();

protected:
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    // set bitmap to the given one if it's ok or to m_bmpNormal and return
    // TRUE if the bitmap really changed
    bool ChangeBitmap(const wxBitmap& bmp);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
};

#endif // _WX_UNIV_BMPBUTTN_H_

