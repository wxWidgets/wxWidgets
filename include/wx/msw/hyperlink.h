/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/hyperlink.h
// Purpose:     Hyperlink control
// Author:      Rickard Westerlund
// Created:     2010-08-04
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_HYPERLINK_H_
#define _WX_MSW_HYPERLINK_H_

#include "wx/generic/hyperlink.h"

// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxHyperlinkCtrl : public wxGenericHyperlinkCtrl
{
public:
    // Default constructor (for two-step construction).
    wxHyperlinkCtrl() = default;

    // Constructor.
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label, const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr))
    {
        (void)Create(parent, id, label, url, pos, size, style, name);
    }

    // Creation function (for two-step construction).
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label, const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));

    virtual ~wxHyperlinkCtrl();

    // overridden base class methods
    // -----------------------------

    virtual void SetURL(const wxString &url) override;

    virtual void SetLabel(const wxString &label) override;

    // Native control doesn't change appearance on hover, so we don't support
    // changing hover colour.
    virtual wxColour GetHoverColour() const override;

    virtual wxColour GetNormalColour() const override;
    virtual void SetNormalColour(const wxColour &colour) override;

    virtual wxColour GetVisitedColour() const override;
    virtual void SetVisitedColour(const wxColour &colour) override;

    // overridden/inherited wxWindow methods
    virtual wxVisualAttributes GetDefaultAttributes() const override;
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

protected:
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;
    virtual wxSize DoGetBestClientSize() const override;

private:
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result) override;

    bool MSWAreCustomColoursEnabled() const;
    void MSWEnableCustomColours();

    wxDECLARE_DYNAMIC_CLASS( wxHyperlinkCtrl );
};

#endif // _WX_MSW_HYPERLINK_H_
