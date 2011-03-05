/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/statline.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATLINE_H_
#define _WX_QT_STATLINE_H_

#include "wx/qt/pointer_qt.h"
#include <QtGui/QFrame>

class WXDLLIMPEXP_CORE wxStaticLine : public wxStaticLineBase
{
public:
    wxStaticLine();

    wxStaticLine( wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString &name = wxStaticLineNameStr );

    bool Create( wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxLI_HORIZONTAL,
                 const wxString &name = wxStaticLineNameStr );

    virtual QFrame *GetHandle() const;

private:
    wxQtPointer< QFrame > m_qtFrame;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxStaticLine );
};

#endif // _WX_QT_STATLINE_H_
