/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/filepickerg.h
// Purpose:     wxGenericFileDirButton, wxGenericFileButton, wxGenericDirButton
// Author:      Francesco Montorsi
// Modified by:
// Created:     14/4/2006
// Copyright:   (c) Francesco Montorsi
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDIRPICKER_H_
#define _WX_FILEDIRPICKER_H_

#include "wx/filename.h"
#include "wx/filedlg.h"
#include "wx/dirdlg.h"


extern const wxEventType wxEVT_COMMAND_DIRPICKER_CHANGED;
extern const wxEventType wxEVT_COMMAND_FILEPICKER_CHANGED;


//-----------------------------------------------------------------------------
// wxGenericFileDirButton: a button which brings up a wx{File|Dir}Dialog
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericFileDirButton : public wxButton,
                                                public wxFileDirPickerWidgetBase
{
public:
    wxGenericFileDirButton() { m_dialog = NULL; }
    wxGenericFileDirButton(wxWindow *parent,
                           wxWindowID id,
                           const wxString& label = wxFilePickerWidgetLabel,
                           const wxString& path = wxEmptyString,
                           const wxString &message = wxFileSelectorPromptStr,
                           const wxString &wildcard = wxFileSelectorDefaultWildcardStr,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = 0,
                           const wxValidator& validator = wxDefaultValidator,
                           const wxString& name = wxFilePickerWidgetNameStr)
    {
        m_dialog = NULL;
        Create(parent, id, label, path, message, wildcard,
               pos, size, style, validator, name);
    }

    virtual ~wxGenericFileDirButton() {}

public:     // overrideable

    virtual bool CreateDialog(const wxString &message,
                              const wxString &wildcard) = 0;

    // NULL is because of a problem with destruction order in both generic & GTK code
    virtual wxWindow *GetDialogParent()
        { return NULL; }

    virtual wxEventType GetEventType() const = 0;

public:

    bool Destroy()
    {
        m_dialog->Destroy();
        return wxButton::Destroy();
    }

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxFilePickerWidgetLabel,
           const wxString& path = wxEmptyString,
           const wxString &message = wxFileSelectorPromptStr,
           const wxString &wildcard = wxFileSelectorDefaultWildcardStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxFilePickerWidgetNameStr);

    // event handler for the click
    void OnButtonClick(wxCommandEvent &);

    wxDialog *m_dialog;
};


//-----------------------------------------------------------------------------
// wxGenericFileButton: a button which brings up a wxFileDialog
//-----------------------------------------------------------------------------

#define wxFILEBTN_DEFAULT_STYLE                     wxFLP_OPEN

class WXDLLIMPEXP_CORE wxGenericFileButton : public wxGenericFileDirButton
{
public:
    wxGenericFileButton() {}
    wxGenericFileButton(wxWindow *parent,
                        wxWindowID id,
                        const wxString& label = wxFilePickerWidgetLabel,
                        const wxString& path = wxEmptyString,
                        const wxString &message = wxFileSelectorPromptStr,
                        const wxString &wildcard = wxFileSelectorDefaultWildcardStr,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxFILEBTN_DEFAULT_STYLE,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxFilePickerWidgetNameStr)
    {
        Create(parent, id, label, path, message, wildcard,
               pos, size, style, validator, name);
    }

public:     // overrideable

    virtual long GetDialogStyle() const
    {
        long filedlgstyle = 0;

        if (this->HasFlag(wxFLP_OPEN))
            filedlgstyle |= wxFD_OPEN;
        if (this->HasFlag(wxFLP_SAVE))
            filedlgstyle |= wxFD_SAVE;
        if (this->HasFlag(wxFLP_OVERWRITE_PROMPT))
            filedlgstyle |= wxFD_OVERWRITE_PROMPT;
        if (this->HasFlag(wxFLP_FILE_MUST_EXIST))
            filedlgstyle |= wxFD_FILE_MUST_EXIST;
        if (this->HasFlag(wxFLP_CHANGE_DIR))
            filedlgstyle |= wxFD_CHANGE_DIR;

        return filedlgstyle;
    }

    virtual bool CreateDialog(const wxString &message, const wxString &wildcard)
    {
        m_dialog = new wxFileDialog(GetDialogParent(), message,
                                    wxEmptyString, wxEmptyString,
                                    wildcard, GetDialogStyle());

        // this sets both the default folder and the default file of the dialog
        GetDialog()->SetPath(m_path);

        return true;
    }

    wxFileDialog *GetDialog()
        { return wxStaticCast(m_dialog, wxFileDialog); }
    void UpdateDialogPath()
        { GetDialog()->SetPath(m_path); }
    void UpdatePathFromDialog()
        { m_path = GetDialog()->GetPath(); }
    wxEventType GetEventType() const
        { return wxEVT_COMMAND_FILEPICKER_CHANGED; }

private:
    DECLARE_DYNAMIC_CLASS(wxGenericFileButton)
};


//-----------------------------------------------------------------------------
// wxGenericDirButton: a button which brings up a wxDirDialog
//-----------------------------------------------------------------------------

#define wxDIRBTN_DEFAULT_STYLE                     0

class WXDLLIMPEXP_CORE wxGenericDirButton : public wxGenericFileDirButton
{
public:
    wxGenericDirButton() {}
    wxGenericDirButton(wxWindow *parent,
                       wxWindowID id,
                       const wxString& label = wxDirPickerWidgetLabel,
                       const wxString& path = wxEmptyString,
                       const wxString &message = wxDirSelectorPromptStr,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDIRBTN_DEFAULT_STYLE,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxDirPickerWidgetNameStr)
    {
        Create(parent, id, label, path, message, wxEmptyString,
               pos, size, style, validator, name);
    }

public:     // overrideable

    virtual long GetDialogStyle() const
    {
        long dirdlgstyle = 0;

        if (this->HasFlag(wxDIRP_DIR_MUST_EXIST))
            dirdlgstyle |= wxDD_DIR_MUST_EXIST;
        if (this->HasFlag(wxDIRP_CHANGE_DIR))
            dirdlgstyle |= wxDD_CHANGE_DIR;

        return dirdlgstyle;
    }

    virtual bool CreateDialog(const wxString &message, const wxString &WXUNUSED(wildcard))
    {
        m_dialog = new wxDirDialog(GetDialogParent(), message, m_path,
                                   GetDialogStyle());
        return true;
    }

    wxDirDialog *GetDialog()
        { return wxStaticCast(m_dialog, wxDirDialog); }
    void UpdateDialogPath()
        { GetDialog()->SetPath(m_path); }
    void UpdatePathFromDialog()
        { m_path = GetDialog()->GetPath(); }
    wxEventType GetEventType() const
        { return wxEVT_COMMAND_DIRPICKER_CHANGED; }

private:
    DECLARE_DYNAMIC_CLASS(wxGenericDirButton)
};


#endif // _WX_FILEDIRPICKER_H_
