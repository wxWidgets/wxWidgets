/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/filedirpicker.h
// Purpose:     wxFileButton, wxDirButton header
// Author:      Francesco Montorsi
// Modified by:
// Created:     14/4/2006
// Copyright:   (c) Francesco Montorsi
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_FILEPICKER_H_
#define _WX_GTK_FILEPICKER_H_

// since GtkColorButton is available only for GTK+ >= 2.4,
// we need to use generic versions if we detect (at runtime)
// that GTK+ < 2.4
#include "wx/generic/filepickerg.h"

//-----------------------------------------------------------------------------
// wxFileButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFileButton : public wxGenericFileButton
{
public:
    wxFileButton() {}
    wxFileButton(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label = wxFilePickerWidgetLabel,
                 const wxString &path = wxEmptyString,
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

    virtual ~wxFileButton() ;


public:     // overrides

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxFilePickerWidgetLabel,
                const wxString &path = wxEmptyString,
                const wxString &message = wxFileSelectorPromptStr,
                const wxString &wildcard = wxFileSelectorDefaultWildcardStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxFilePickerWidgetNameStr);

    // event handler for the click
    void OnDialogOK(wxCommandEvent &);


    // GtkFileChooserButton does not support GTK_FILE_CHOOSER_ACTION_SAVE
    // so we replace it with GTK_FILE_CHOOSER_ACTION_OPEN
    long GetDialogStyle() const
    {
        return (wxGenericFileButton::GetDialogStyle() & ~wxFD_SAVE) | wxFD_OPEN;
    }


private:
    DECLARE_DYNAMIC_CLASS(wxFileButton)
};


//-----------------------------------------------------------------------------
// wxDirButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDirButton : public wxGenericDirButton
{
public:
    wxDirButton() {}
    wxDirButton(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxFilePickerWidgetLabel,
                const wxString &path = wxEmptyString,
                const wxString &message = wxFileSelectorPromptStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxFilePickerWidgetNameStr)
    {
        Create(parent, id, label, path, message, wxEmptyString,
                pos, size, style, validator, name);
    }

    virtual ~wxDirButton();


public:     // overrides

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxFilePickerWidgetLabel,
                const wxString &path = wxEmptyString,
                const wxString &message = wxFileSelectorPromptStr,
                const wxString &wildcard = wxFileSelectorDefaultWildcardStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxFilePickerWidgetNameStr);

    // used by the GTK callback only
    void UpdatePath(char *gtkpath)
        { m_path = wxString::FromAscii(gtkpath); }

    // GtkFileChooserButton does not support GTK_FILE_CHOOSER_CREATE_FOLDER
    // thus we must ensure that the wxDD_DIR_MUST_EXIST style was given
    long GetDialogStyle() const
    {
        return (wxGenericDirButton::GetDialogStyle() | wxDD_DIR_MUST_EXIST);
    }

private:
    DECLARE_DYNAMIC_CLASS(wxDirButton)
};

#endif // _WX_GTK_FILEPICKER_H_

