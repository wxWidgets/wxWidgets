/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/filedlgg.h
// Purpose:     wxGenericFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     8/17/99
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLGG_H_
#define _WX_FILEDLGG_H_

#include "wx/listctrl.h"
#include "wx/datetime.h"
#include "wx/filefn.h"
#include "wx/filedlg.h"
#include "wx/generic/filectrlg.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxBitmapButton;
class WXDLLIMPEXP_FWD_CORE wxGenericFileCtrl;
class WXDLLIMPEXP_FWD_CORE wxGenericFileDialog;
class WXDLLIMPEXP_FWD_CORE wxFileCtrlEvent;

//-------------------------------------------------------------------------
// wxGenericFileDialog
//-------------------------------------------------------------------------

class WXDLLEXPORT wxGenericFileDialog: public wxFileDialogBase
{
public:
    wxGenericFileDialog() : wxFileDialogBase() { Init(); }

    wxGenericFileDialog(wxWindow *parent,
                        const wxString& message = wxFileSelectorPromptStr,
                        const wxString& defaultDir = wxEmptyString,
                        const wxString& defaultFile = wxEmptyString,
                        const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                        long style = wxFD_DEFAULT_STYLE,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& sz = wxDefaultSize,
                        const wxString& name = wxFileDialogNameStr,
                        bool bypassGenericImpl = false );

    bool Create( wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxFileDialogNameStr,
                 bool bypassGenericImpl = false );

    virtual ~wxGenericFileDialog();

    virtual void SetMessage(const wxString& message) { SetTitle(message); }
    virtual void SetPath(const wxString& path);
    virtual void SetFilterIndex(int filterIndex);
    virtual void SetWildcard(const wxString& wildCard);

    // for multiple file selection
    virtual void GetPaths(wxArrayString& paths) const;
    virtual void GetFilenames(wxArrayString& files) const;

    // implementation only from now on
    // -------------------------------

    virtual int ShowModal();
    virtual bool Show( bool show = true );

    void OnList( wxCommandEvent &event );
    void OnReport( wxCommandEvent &event );
    void OnUp( wxCommandEvent &event );
    void OnHome( wxCommandEvent &event );
    void OnOk( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnFileActivated( wxFileCtrlEvent &event);

    virtual void UpdateControls();

private:
    // Don't use this implementation at all :-)
    bool m_bypassGenericImpl;

protected:

    wxString               m_filterExtension;
    wxGenericFileCtrl     *m_filectrl;
    wxBitmapButton        *m_upDirButton;
    wxBitmapButton        *m_newDirButton;

private:
    void Init();
    DECLARE_DYNAMIC_CLASS(wxGenericFileDialog)
    DECLARE_EVENT_TABLE()

    // these variables are preserved between wxGenericFileDialog calls
    static long ms_lastViewStyle;     // list or report?
    static bool ms_lastShowHidden;    // did we show hidden files?
};

#ifdef wxHAS_GENERIC_FILEDIALOG

class WXDLLEXPORT wxFileDialog: public wxGenericFileDialog
{
public:
     wxFileDialog() {}

    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition)
          :wxGenericFileDialog(parent, message, defaultDir, defaultFile, wildCard, style, pos)
     {
     }

private:
     DECLARE_DYNAMIC_CLASS(wxFileDialog)
};

#endif // wxHAS_GENERIC_FILEDIALOG

#endif // _WX_FILEDLGG_H_
