/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/button.h
// Purpose:     wxButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#ifdef __GNUG__
#pragma interface "button.h"
#endif

// ----------------------------------------------------------------------------
// Pushbutton
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxButton : public wxButtonBase
{
public:
    wxButton() { }
    wxButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    virtual ~wxButton();

    virtual void SetDefault();

    // implementation from now on
    virtual void Command(wxCommandEvent& event);
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
    virtual bool MSWCommand(WXUINT param, WXWORD id);

#ifdef __WIN32__
    // coloured buttons support
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);

    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);

private:
    void MakeOwnerDrawn();
#endif // __WIN32__

protected:
    // send a notification event, return TRUE if processed
    bool SendClickEvent();

    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_DYNAMIC_CLASS(wxButton)
};

#endif
    // _WX_BUTTON_H_
