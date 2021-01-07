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

    virtual int GetThumbPosition() const wxOVERRIDE;
    virtual int GetThumbSize() const wxOVERRIDE;
    virtual int GetPageSize() const wxOVERRIDE;
    virtual int GetRange() const wxOVERRIDE;

    virtual void SetThumbPosition(int viewStart) wxOVERRIDE;
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true) wxOVERRIDE;

    QScrollBar *GetQScrollBar() const { return m_qtScrollBar; }
    QWidget *GetHandle() const wxOVERRIDE;

private:
    QScrollBar *m_qtScrollBar;

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
};


#endif // _WX_QT_SCROLBAR_H_
