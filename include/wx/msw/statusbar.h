///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/statbr.h
// Purpose:     native implementation of wxStatusBar
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.04.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STATUSBAR_H_
#define _WX_MSW_STATUSBAR_H_

#if wxUSE_NATIVE_STATUSBAR

class WXDLLIMPEXP_FWD_CORE wxClientDC;

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    // ctors and such
    wxStatusBar();
    wxStatusBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxStatusBarNameStr)
    {
        m_pDC = NULL;
        (void)Create(parent, id, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxStatusBarNameStr);

    virtual ~wxStatusBar();

    // a status line can have several (<256) fields numbered from 0
    virtual void SetFieldsCount(int number = 1, const int *widths = NULL);

    // each field of status line has it's own text
    virtual void     SetStatusText(const wxString& text, int number = 0);

    // set status line fields' widths
    virtual void SetStatusWidths(int n, const int widths_field[]);

    // set status line fields' styles
    virtual void SetStatusStyles(int n, const int styles[]);

    // sets the minimal vertical size of the status bar
    virtual void SetMinHeight(int height);

    // get the position and size of the field's internal bounding rectangle
    virtual bool GetFieldRect(int i, wxRect& rect) const;

    // get the border size
    virtual int GetBorderX() const;
    virtual int GetBorderY() const;

    virtual bool SetFont(const wxFont& font);

    virtual WXLRESULT MSWWindowProc(WXUINT nMsg,
                                    WXWPARAM wParam,
                                    WXLPARAM lParam);
protected:
    void CopyFieldsWidth(const int widths[]);
    void SetFieldsWidth();
    void UpdateFieldText(int nField);

    // override some base class virtuals
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // used by UpdateFieldText
    wxClientDC *m_pDC;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBar)
};

#endif  // wxUSE_NATIVE_STATUSBAR

#endif // _WX_MSW_STATUSBAR_H_
