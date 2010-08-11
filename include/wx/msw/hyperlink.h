/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/hyperlink.h
// Purpose:     Hyperlink control
// Author:      Rickard Westerlund
// Created:     2010-08-04
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HYPERLINKCTRL_H_
#define _WX_HYPERLINKCTRL_H_

#include "wx/generic/hyperlink.h"

// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxHyperlinkCtrl : public wxGenericHyperlinkCtrl
{
public:
    // Default constructor (for two-step construction).
    wxHyperlinkCtrl() { }

    // Constructor.
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label, const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxHyperlinkCtrlNameStr)
    {
        (void) Create(parent, id, label, url, pos, size, style, name);
    }

    // Creation function (for two-step construction).
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label, const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxHyperlinkCtrlNameStr);


    // overrides methods in base class
    virtual void SetURL(const wxString &url);

    virtual void SetLabel(const wxString &label);

protected:
    // overridden base class virtuals

    // Returns the best size for the window, which is the size needed
    // to display the text label.
    virtual wxSize DoGetBestClientSize() const;

private:
    // overrides base class method
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    wxDECLARE_DYNAMIC_CLASS( wxHyperlinkCtrl );
};

#endif // _WX_HYPERLINKCTRL_H_
