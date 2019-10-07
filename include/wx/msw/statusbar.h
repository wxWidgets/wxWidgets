///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/statusbar.h
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.04.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STATUSBAR_H_
#define _WX_MSW_STATUSBAR_H_

#if wxUSE_NATIVE_STATUSBAR

#include "wx/vector.h"
#include "wx/tooltip.h"

class WXDLLIMPEXP_FWD_CORE wxClientDC;

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    // ctors and such
    wxStatusBar();
    wxStatusBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxStatusBarNameStr)
    {
        m_pDC = NULL;
        (void)Create(parent, id, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxStatusBarNameStr);

    virtual ~wxStatusBar();

    // implement base class methods
    virtual void SetFieldsCount(int number = 1, const int *widths = NULL) wxOVERRIDE;
    virtual void SetStatusWidths(int n, const int widths_field[]) wxOVERRIDE;
    virtual void SetStatusStyles(int n, const int styles[]) wxOVERRIDE;
    virtual void SetMinHeight(int height) wxOVERRIDE;
    virtual bool GetFieldRect(int i, wxRect& rect) const wxOVERRIDE;

    virtual int GetBorderX() const wxOVERRIDE;
    virtual int GetBorderY() const wxOVERRIDE;

    // override some wxWindow virtual methods too
    virtual bool SetFont(const wxFont& font) wxOVERRIDE;

    virtual WXLRESULT MSWWindowProc(WXUINT nMsg,
                                    WXWPARAM wParam,
                                    WXLPARAM lParam) wxOVERRIDE;

protected:
    // implement base class pure virtual method
    virtual void DoUpdateStatusText(int number) wxOVERRIDE;

    // override some base class virtuals
    virtual WXDWORD MSWGetStyle(long flags, WXDWORD *exstyle = NULL) const wxOVERRIDE;
    virtual wxSize DoGetBestSize() const wxOVERRIDE;
    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;
#if wxUSE_TOOLTIPS
    virtual bool MSWProcessMessage(WXMSG* pMsg) wxOVERRIDE;
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM* result) wxOVERRIDE;
#endif

    // implementation of the public SetStatusWidths()
    void MSWUpdateFieldsWidths();

    virtual void MSWUpdateFontOnDPIChange(const wxSize& newDPI) wxOVERRIDE;

    // used by DoUpdateStatusText()
    wxClientDC *m_pDC;

#if wxUSE_TOOLTIPS
    // the tooltips used when wxSTB_SHOW_TIPS is given
    wxVector<wxToolTip*> m_tooltips;
#endif

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

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBar);
};

#endif  // wxUSE_NATIVE_STATUSBAR

#endif // _WX_MSW_STATUSBAR_H_
