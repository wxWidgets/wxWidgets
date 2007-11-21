/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.h
// Purpose:     wxDirDialog
// Author:      Francesco Montorsi
// Id:          $Id$
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDIRDLGH__
#define __GTKDIRDLGH__

#include "wx/generic/dirdlgg.h"

//-------------------------------------------------------------------------
// wxDirDialog
//-------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDirDialog : public wxGenericDirDialog
{
public:
    wxDirDialog() { }

    wxDirDialog(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

    virtual ~wxDirDialog() { }


public:     // overrides from wxGenericDirDialog

    wxString GetPath() const;
    void SetPath(const wxString& path);

    virtual int ShowModal();
    virtual bool Show( bool show = true );


protected:
    // override this from wxTLW since the native
    // form doesn't have any m_wxwindow
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);


private:
    DECLARE_DYNAMIC_CLASS(wxDirDialog)
    DECLARE_EVENT_TABLE()
    void OnFakeOk( wxCommandEvent &event );
};

#endif // __GTKDIRDLGH__
