/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/filedlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FILEDLG_H_
#define _WX_QT_FILEDLG_H_

class WXDLLIMPEXP_CORE wxFileDialog : public wxFileDialogBase
{
public:
    wxFileDialog();
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxFileDialogNameStr);

protected:

private:
};

#endif // _WX_QT_FILEDLG_H_
