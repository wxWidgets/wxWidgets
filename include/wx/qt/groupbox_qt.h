/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/groupbox_qt.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_GROUPBOX_QT_H_
#define _WX_QT_GROUPBOX_QT_H_

#include <QtGui/QGroupBox>

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxString;


class WXDLLIMPEXP_CORE wxQtGroupBox : public QGroupBox
{
public:
    wxQtGroupBox( wxWindow *parent, const wxString &label );
};

#endif /* _WX_QT_GROUPBOX_QT_H_ */
