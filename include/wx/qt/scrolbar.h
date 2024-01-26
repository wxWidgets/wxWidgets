/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/scrolbar.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_SCROLBAR_H_
#define _WX_QT_SCROLBAR_H_

#include "wx/scrolbar.h"

class QScrollBar;

class WXDLLIMPEXP_FWD_CORE wxQtScrollBar;

class WXDLLIMPEXP_CORE wxScrollBar : public wxScrollBarBase
{
public:
    wxScrollBar();
    wxScrollBar( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxScrollBarNameStr) );

    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxScrollBarNameStr) );

    virtual int GetThumbPosition() const override;
    virtual int GetThumbSize() const override;
    virtual int GetPageSize() const override;
    virtual int GetRange() const override;

    virtual void SetThumbPosition(int viewStart) override;
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true) override;

    QScrollBar *GetQScrollBar() const { return m_qtScrollBar; }
    QWidget *GetHandle() const override;

private:
    QScrollBar *m_qtScrollBar;

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
};


#endif // _WX_QT_SCROLBAR_H_
