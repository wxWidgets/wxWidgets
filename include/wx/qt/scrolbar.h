/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/scrolbar.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_SCROLBAR_H_
#define _WX_QT_SCROLBAR_H_

#include "wx/scrolbar.h"

#include <QtWidgets/QScrollBar>

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
           const wxString& name = wxScrollBarNameStr );

    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxScrollBarNameStr );

    virtual int GetThumbPosition() const;
    virtual int GetThumbSize() const;
    virtual int GetPageSize() const;
    virtual int GetRange() const;

    virtual void SetThumbPosition(int viewStart);
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true);

    virtual QScrollBar* GetHandle() const;

private:
    QScrollBar *m_qtScrollBar;

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
};


#endif // _WX_QT_SCROLBAR_H_
