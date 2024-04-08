/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/filedirpicker.h
// Purpose:     wxFileButton, wxDirButton header
// Author:      Francesco Montorsi
// Created:     14/4/2006
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_FILEPICKER_H_
#define _WX_GTK_FILEPICKER_H_

class WXDLLIMPEXP_FWD_CORE wxFileDialog;
class WXDLLIMPEXP_FWD_CORE wxDirDialog;

class WXDLLIMPEXP_FWD_CORE wxGenericFileButton;
class WXDLLIMPEXP_FWD_CORE wxGenericDirButton;

extern WXDLLIMPEXP_DATA_CORE(const char) wxFileSelectorPromptStr[];
extern WXDLLIMPEXP_DATA_CORE(const char) wxFileSelectorDefaultWildcardStr[];

//-----------------------------------------------------------------------------
// wxFileButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFileButton : public wxControl,
                                      public wxFileDirPickerWidgetBase
{
public:
    wxFileButton() = default;
    wxFileButton(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label = wxASCII_STR(wxFilePickerWidgetLabel),
                 const wxString &path = wxEmptyString,
                 const wxString &message = wxASCII_STR(wxFileSelectorPromptStr),
                 const wxString &wildcard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxFILEBTN_DEFAULT_STYLE,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxASCII_STR(wxFilePickerWidgetNameStr))
    {
        Create(parent, id, label, path, message, wildcard,
               pos, size, style, validator, name);
    }

    virtual ~wxFileButton();


public:     // overrides

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxASCII_STR(wxFilePickerWidgetLabel),
                const wxString &path = wxEmptyString,
                const wxString &message = wxASCII_STR(wxFileSelectorPromptStr),
                const wxString &wildcard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxFilePickerWidgetNameStr));

    // event handler for the click
    void OnDialogOK(wxCommandEvent &);

    virtual wxString GetPath() const override;
    virtual void SetPath(const wxString &str) override;
    virtual void SetInitialDirectory(const wxString& dir) override;

    virtual wxControl* AsControl() override { return this; }

protected:
    wxFileDialog* m_dialog = nullptr;

private:
    wxGenericFileButton* m_genericButton = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxFileButton);
};


//-----------------------------------------------------------------------------
// wxDirButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDirButton : public wxControl,
                                     public wxFileDirPickerWidgetBase
{
public:
    wxDirButton() = default;
    wxDirButton(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxASCII_STR(wxFilePickerWidgetLabel),
                const wxString &path = wxEmptyString,
                const wxString &message = wxASCII_STR(wxFileSelectorPromptStr),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxFilePickerWidgetNameStr))
    {
        Create(parent, id, label, path, message,
                pos, size, style, validator, name);
    }

    virtual ~wxDirButton();


public:     // overrides

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxASCII_STR(wxFilePickerWidgetLabel),
                const wxString &path = wxEmptyString,
                const wxString &message = wxASCII_STR(wxFileSelectorPromptStr),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxFilePickerWidgetNameStr));


    virtual wxString GetPath() const override;
    virtual void SetPath(const wxString &str) override;
    virtual void SetInitialDirectory(const wxString& dir) override;

    virtual wxControl* AsControl() override { return this; }

protected:
    wxDirDialog* m_dialog = nullptr;

public:    // used by the GTK callback only

    bool m_bIgnoreNextChange = false;

private:
    wxGenericDirButton* m_genericButton = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxDirButton);
};

#endif // _WX_GTK_FILEPICKER_H_

