/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colordlg.h
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COLORDLG_H_
#define _WX_QT_COLORDLG_H_

#include "wx/dialog.h"

#include <QtWidgets/QColorDialog>

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog() { }
    wxColourDialog(wxWindow *parent,
                   wxColourData *data = NULL) { Create(parent, data); }

    bool Create(wxWindow *parent, wxColourData *data = NULL);

    wxColourData &GetColourData();

    QColorDialog *GetHandle() const { return static_cast<QColorDialog *>(m_qtWindow); }

private:

    wxColourData m_data;
};

#endif // _WX_QT_COLORDLG_H_
