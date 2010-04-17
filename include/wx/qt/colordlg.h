/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colordlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COLORDLG_H_
#define _WX_QT_COLORDLG_H_

#include "wx/dialog.h"
#include "wx/cmndata.h"
#include <QtGui/QColorDialog>

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog();
    wxColourDialog(wxWindow *parent,
                   wxColourData *data = NULL);

    bool Create(wxWindow *parent, wxColourData *data = NULL);

    wxColourData &GetColourData();

    virtual int ShowModal();

protected:
    virtual QColorDialog *GetHandle() const;
    
private:
};

#endif // _WX_QT_COLORDLG_H_
