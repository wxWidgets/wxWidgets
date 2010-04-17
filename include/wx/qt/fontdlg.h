/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/fontdlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FONTDLG_H_
#define _WX_QT_FONTDLG_H_

#include <QtGui/QFontDialog>

class WXDLLIMPEXP_CORE wxFontDialog : public wxFontDialogBase
{
public:
    wxFontDialog();
    wxFontDialog(wxWindow *parent);
    wxFontDialog(wxWindow *parent, const wxFontData& data);
    
protected:
    virtual QFontDialog *GetHandle() const;
};

#endif // _WX_QT_FONTDLG_H_
