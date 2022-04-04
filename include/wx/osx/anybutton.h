/////////////////////////////////////////////////////////////////////////////
// Name:        anybutton.h
// Purpose:     wxAnyButton class
// Author:      Stefan Csomor
// Created:     1998-01-01 (extracted from button.h)
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_ANYBUTTON_H_
#define _WX_OSX_ANYBUTTON_H_

// Any button
class WXDLLIMPEXP_CORE wxAnyButton : public wxAnyButtonBase
{
public:
    wxAnyButton() {}

    static wxSize GetDefaultSize();

    virtual void SetLabel(const wxString& label) wxOVERRIDE;

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    void OnEnterWindow( wxMouseEvent& event);
    void OnLeaveWindow( wxMouseEvent& event);

    virtual wxBitmap DoGetBitmap(State which) const wxOVERRIDE;
    virtual void DoSetBitmap(const wxBitmapBundle& bitmapBundle, State which) wxOVERRIDE;
    virtual void DoSetBitmapPosition(wxDirection dir) wxOVERRIDE;

    virtual void DoSetBitmapMargins(int x, int y) wxOVERRIDE
    {
        m_marginX = x;
        m_marginY = y;
        InvalidateBestSize();
    }

#if wxUSE_MARKUP && wxOSX_USE_COCOA
    virtual bool DoSetLabelMarkup(const wxString& markup) wxOVERRIDE;
#endif // wxUSE_MARKUP && wxOSX_USE_COCOA


    // the margins around the bitmap
    int m_marginX;
    int m_marginY;

    // the bitmaps for the different state of the buttons, all of them may be
    // invalid and the button only shows a bitmap at all if State_Normal bitmap
    // is valid
    wxBitmapBundle m_bitmaps[State_Max];

    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_OSX_ANYBUTTON_H_
