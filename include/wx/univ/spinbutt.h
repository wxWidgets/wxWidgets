///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/spinbutt.h
// Purpose:     universal version of wxSpinButton
// Author:      Vadim Zeitlin
// Modified by:
// Created:     21.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_SPINBUTT_H_
#define _WX_UNIV_SPINBUTT_H_

// ----------------------------------------------------------------------------
// wxSpinButton
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton() { }
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                 const wxString& name = )
    {
        (void)Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                const wxString& name = wxSPIN_BUTTON_NAME);

    // implement wxSpinButtonBase methods
    virtual int GetValue() const;
    virtual void SetValue(int val);

protected:
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    virtual wxString GetInputHandlerType() const;

    // the currnt controls value
    int m_value;
};

#endif // _WX_UNIV_SPINBUTT_H_

