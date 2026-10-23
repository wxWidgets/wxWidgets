///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/richtooltip.h
// Purpose:     wxRichToolTipGenericImpl declaration.
// Author:      Vadim Zeitlin
// Created:     2011-10-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PRIVATE_RICHTOOLTIP_H_
#define _WX_GENERIC_PRIVATE_RICHTOOLTIP_H_

#include "wx/private/richtooltip.h"

#include "wx/bmpbndl.h"
#include "wx/colour.h"
#include "wx/font.h"

class WXDLLIMPEXP_FWD_CORE wxSizer;
class WXDLLIMPEXP_FWD_CORE wxWindow;

// ----------------------------------------------------------------------------
// wxRichToolTipContents: creates the windows showing a tooltip contents.
// ----------------------------------------------------------------------------

// Also used by other controls showing rich tooltips in their own popups. The
// parent must handle wxEVT_HYPERLINK from the help link without skipping it.
class WXDLLIMPEXP_CORE wxRichToolTipContents
{
public:
    wxBitmapBundle m_icon;
    wxFont m_titleFont;

    // Unset colors mean using the parent ones.
    wxColour m_textColour,
             m_backgroundColour;

    // In DIPs, -1 to not wrap.
    int m_wrapWidth = -1;

    bool m_centreMessage = false;

    wxSizer* Create(wxWindow* parent,
                    const wxRichToolTipInfo& info,
                    bool disabled) const;
};

// ----------------------------------------------------------------------------
// wxRichToolTipGenericImpl: defines generic wxRichToolTip implementation.
// ----------------------------------------------------------------------------

class wxRichToolTipGenericImpl : public wxRichToolTipImpl
{
public:
    explicit wxRichToolTipGenericImpl(const wxRichToolTipInfo& info) :
        m_info(info)
    {
        m_tipKind = wxTipKind_Auto;

        // This is pretty arbitrary, we could follow MSW and use some multiple
        // of double-click time here.
        m_timeout = 5000;
        m_delay = 0;
    }

    virtual void SetBackgroundColour(const wxColour& col,
                                     const wxColour& colEnd) override;
    virtual void SetCustomIcon(const wxBitmapBundle& icon) override;
    virtual void SetStandardIcon(int icon) override;
    virtual void SetTimeout(unsigned milliseconds,
                            unsigned millisecondsDelay = 0) override;
    virtual void SetTipKind(wxTipKind tipKind) override;
    virtual void SetTitleFont(const wxFont& font) override;

    virtual void ShowFor(wxWindow* win, const wxRect* rect = nullptr) override;

protected:
    const wxRichToolTipInfo m_info;

private:
    wxBitmapBundle m_icon;

    wxColour m_colStart,
             m_colEnd;

    unsigned m_timeout,
             m_delay;

    wxTipKind m_tipKind;

    wxFont m_titleFont;
};

#endif // _WX_GENERIC_PRIVATE_RICHTOOLTIP_H_
