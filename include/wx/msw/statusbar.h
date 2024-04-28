///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/statusbar.h
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Created:     04.04.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STATUSBAR_H_
#define _WX_MSW_STATUSBAR_H_

#if wxUSE_NATIVE_STATUSBAR

#include "wx/tooltip.h"

#include <memory>
#include <vector>

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    // ctors and such
    wxStatusBar();
    wxStatusBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr))
    {
        (void)Create(parent, id, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    virtual ~wxStatusBar();

    // implement base class methods
    virtual void SetFieldsCount(int number = 1, const int *widths = nullptr) override;
    virtual void SetStatusWidths(int n, const int widths_field[]) override;
    virtual void SetStatusStyles(int n, const int styles[]) override;
    virtual void SetMinHeight(int height) override;
    virtual bool GetFieldRect(int i, wxRect& rect) const override;

    virtual int GetBorderX() const override;
    virtual int GetBorderY() const override;

    // override some wxWindow virtual methods too
    virtual wxVisualAttributes GetDefaultAttributes() const override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    virtual WXLRESULT MSWWindowProc(WXUINT nMsg,
                                    WXWPARAM wParam,
                                    WXLPARAM lParam) override;

protected:
    // implement base class pure virtual method
    virtual void DoUpdateStatusText(int number) override;

    // override some base class virtuals
    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = nullptr) const override;
    virtual wxSize DoGetBestSize() const override;
    virtual void DoMoveWindow(int x, int y, int width, int height) override;
#if wxUSE_TOOLTIPS
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM* result) override;
#endif

    virtual bool MSWGetDarkModeSupport(MSWDarkModeSupport& support) const override;

    // implementation of the public SetStatusWidths()
    void MSWUpdateFieldsWidths();

private:
    struct MSWBorders
    {
        int horz,
            vert,
            between;
    };

    // retrieve all status bar borders using SB_GETBORDERS
    MSWBorders MSWGetBorders() const;

    // return the size of the border between the fields
    int MSWGetBorderWidth() const;

    struct MSWMetrics
    {
        int gripWidth,
            textMargin;
    };

    // return the various status bar metrics
    static const MSWMetrics& MSWGetMetrics();

#if wxUSE_TOOLTIPS
    // the tooltips used when wxSTB_SHOW_TIPS is given
    std::vector<std::unique_ptr<wxToolTip>> m_tooltips;
#endif

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBar);
};

#endif  // wxUSE_NATIVE_STATUSBAR

#endif // _WX_MSW_STATUSBAR_H_
