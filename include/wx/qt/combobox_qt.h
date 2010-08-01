/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/combobox_qt.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COMBOBOX_QT_H_
#define _WX_QT_COMBOBOX_QT_H_

#include <QtGui/QComboBox>

class WXDLLIMPEXP_CORE wxQtComboBox : public QComboBox
{
public:
    wxQtComboBox( wxWindow *parent );
    wxQtComboBox( wxWindow *parent, const wxString &value );

    void AddChoices( int count, const wxString choices[] );
    void AddChoices( const wxArrayString &choices );
};

#endif /* _WX_QT_COMBOBOX_QT_H_ */
