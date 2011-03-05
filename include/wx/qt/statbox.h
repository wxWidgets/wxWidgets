/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statbox.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATBOX_H_
#define _WX_QT_STATBOX_H_

#include "wx/qt/pointer_qt.h"
#include "wx/qt/groupbox_qt.h"

class WXDLLIMPEXP_CORE wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox();
    
    wxStaticBox(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxStaticBoxNameStr);

    virtual QGroupBox *GetHandle() const;

protected:

private:
    wxQtPointer< wxQtGroupBox > m_qtGroupBox;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxStaticBox );
};

#endif // _WX_QT_STATBOX_H_
