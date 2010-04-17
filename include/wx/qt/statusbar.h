/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statusbar.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATUSBAR_H_
#define _WX_QT_STATUSBAR_H_

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    wxStatusBar(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxStatusBarNameStr);
    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxStatusBarNameStr);

    virtual bool GetFieldRect(int i, wxRect& rect) const;
    virtual void SetMinHeight(int height);
    virtual int GetBorderX() const;
    virtual int GetBorderY() const;
protected:
    virtual void DoUpdateStatusText(int number);

private:
    void Init();

    DECLARE_DYNAMIC_CLASS( wxStatusBar )
};

#endif // _WX_QT_STATUSBAR_H_
