///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/filedlgcustomize.h
// Purpose:     Private helpers used for wxFileDialog customization
// Author:      Vadim Zeitlin
// Created:     2022-05-26
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_FILEDLGCUSTOMIZE_H_
#define _WX_PRIVATE_FILEDLGCUSTOMIZE_H_

// ----------------------------------------------------------------------------
// wxFileDialogCustomControlImpl: interface for all custom controls
// ----------------------------------------------------------------------------

class wxFileDialogCustomControlImpl
{
public:
    virtual void Show(bool show) = 0;
    virtual void Enable(bool enable) = 0;

    virtual bool DoBind(wxEvtHandler* handler);

    virtual ~wxFileDialogCustomControlImpl();
};

// This class is defined for symmetry with the other ones, but there are no
// button-specific methods so far.
class wxFileDialogButtonImpl : public wxFileDialogCustomControlImpl
{
};

class wxFileDialogCheckBoxImpl : public wxFileDialogCustomControlImpl
{
public:
    virtual bool GetValue() = 0;
    virtual void SetValue(bool value) = 0;
};

#if wxUSE_RADIOBTN
class wxFileDialogRadioButtonImpl : public wxFileDialogCustomControlImpl
{
public:
    virtual bool GetValue() = 0;
    virtual void SetValue(bool value) = 0;
};
#endif // wxUSE_RADIOBTN

class wxFileDialogChoiceImpl : public wxFileDialogCustomControlImpl
{
public:
    virtual int GetSelection() = 0;
    virtual void SetSelection(int n) = 0;
};

class wxFileDialogTextCtrlImpl : public wxFileDialogCustomControlImpl
{
public:
    virtual wxString GetValue() = 0;
    virtual void SetValue(const wxString& value) = 0;
};

class wxFileDialogStaticTextImpl : public wxFileDialogCustomControlImpl
{
public:
    virtual void SetLabelText(const wxString& text) = 0;
};

// ----------------------------------------------------------------------------
// wxFileDialogCustomizeImpl: interface for actual customizers
// ----------------------------------------------------------------------------

class wxFileDialogCustomizeImpl
{
public:
    virtual wxFileDialogButtonImpl* AddButton(const wxString& label) = 0;
    virtual wxFileDialogCheckBoxImpl* AddCheckBox(const wxString& label) = 0;
#if wxUSE_RADIOBTN
    virtual wxFileDialogRadioButtonImpl* AddRadioButton(const wxString& label) = 0;
#endif
    virtual wxFileDialogChoiceImpl* AddChoice(size_t n, const wxString* strings) = 0;
    virtual wxFileDialogTextCtrlImpl* AddTextCtrl(const wxString& label) = 0;
    virtual wxFileDialogStaticTextImpl* AddStaticText(const wxString& label) = 0;

    virtual ~wxFileDialogCustomizeImpl();
};

#endif // _WX_PRIVATE_FILEDLGCUSTOMIZE_H_
