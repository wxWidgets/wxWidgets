/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colordlg.h
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COLORDLG_H_
#define _WX_QT_COLORDLG_H_

#include "wx/dialog.h"

class QColorDialog;

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog() = default;

    explicit wxColourDialog(wxWindow *parent,
                            const wxColourData *data = nullptr) { Create(parent, data); }

    bool Create(wxWindow *parent, const wxColourData *data = nullptr);

    wxColourData &GetColourData();

private:
    QColorDialog *GetQColorDialog() const;

    wxColourData m_data;

    wxDECLARE_DYNAMIC_CLASS(wxColourDialog);
};

#endif // _WX_QT_COLORDLG_H_
