/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.h
// Purpose:     wxDirDialogGTK
// Author:      Francesco Montorsi
// Id:          $Id$
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDIRDLGH__
#define __GTKDIRDLGH__

#include "wx/generic/dirdlgg.h"

//-------------------------------------------------------------------------
// wxDirDialogGTK
//-------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDirDialogGTK : public wxGenericDirDialog
{
public:
    wxDirDialogGTK() { }

    wxDirDialogGTK(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

    virtual ~wxDirDialogGTK();


public:     // overrides from wxGenericDirDialog

    wxString GetPath() const;
    void SetPath(const wxString& path);

    virtual int ShowModal();
    virtual bool Show( bool show = true );


//private:      must be accessible by GTK callback
    bool m_destroyed_by_delete;

protected:
    // override this from wxTLW since the native
    // form doesn't have any m_wxwindow
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);


private:
    DECLARE_DYNAMIC_CLASS(wxDirDialogGTK)
    DECLARE_EVENT_TABLE()
    void OnFakeOk( wxCommandEvent &event );
};

#endif // __GTKDIRDLGH__
