/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/panel.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_PANEL_H_
#define _WX_QT_PANEL_H_

#include "wx/qt/pointer_qt.h"
#include "wx/qt/window_qt.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxPanelNameStr[];

class WXDLLIMPEXP_CORE wxPanel : public wxWindow
{
public:
    wxPanel();
    wxPanel( wxWindow *parent, int x, int y, int width, int height,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr);

    wxPanel(wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr);

    bool Create(wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr);

    virtual ~wxPanel();

    virtual QWidget *GetHandle() const;

private:
    wxQtPointer< wxQtWidget > m_qtWidget;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxPanel );
};

#endif // _WX_QT_PANEL_H_
