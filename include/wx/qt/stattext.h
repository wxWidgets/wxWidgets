/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/stattext.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_STATTEXT_H_
#define _WX_QT_STATTEXT_H_

#include <QtCore/QPointer>
#include <QtGui/QLabel>

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticTextNameStr );
    ~wxStaticText();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxStaticTextNameStr );

    QLabel *GetHandle() const;
private:
    QPointer< QLabel > m_qtLabel;

};

#endif // _WX_QT_STATTEXT_H_
