/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dirdlg.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DIRDLG_H_
#define _WX_QT_DIRDLG_H_

class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
public:
    wxDirDialog();
    wxDirDialog(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

protected:

private:
};

#endif // _WX_QT_DIRDLG_H_
