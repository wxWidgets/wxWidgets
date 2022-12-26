/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/hyperlink.h
// Purpose:     Hyperlink control
// Author:      Francesco Montorsi
// Modified by:
// Created:     14/2/2007
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKHYPERLINKCTRL_H_
#define _WX_GTKHYPERLINKCTRL_H_

#include "wx/generic/hyperlink.h"

// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxHyperlinkCtrl : public wxGenericHyperlinkCtrl
{
    typedef wxGenericHyperlinkCtrl base_type;
public:
    wxHyperlinkCtrl();
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label, const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));

    virtual ~wxHyperlinkCtrl();

    // Creation function (for two-step construction).
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label, const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));


    // get/set
    virtual wxColour GetHoverColour() const override;
    virtual void SetHoverColour(const wxColour &colour) override;

    virtual wxColour GetNormalColour() const override;
    virtual void SetNormalColour(const wxColour &colour) override;

    virtual wxColour GetVisitedColour() const override;
    virtual void SetVisitedColour(const wxColour &colour) override;

    virtual wxString GetURL() const override;
    virtual void SetURL(const wxString &url) override;

    virtual void SetVisited(bool visited = true) override;
    virtual bool GetVisited() const override;

    virtual void SetLabel(const wxString &label) override;

protected:
    virtual wxSize DoGetBestSize() const override;
    virtual wxSize DoGetBestClientSize() const override;

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const override;

    wxDECLARE_DYNAMIC_CLASS(wxHyperlinkCtrl);
};

#endif // _WX_GTKHYPERLINKCTRL_H_
