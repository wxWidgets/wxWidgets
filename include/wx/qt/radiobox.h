/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/radiobox.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_RADIOBOX_H_
#define _WX_QT_RADIOBOX_H_

class QGroupBox;
class QButtonGroup;
class QGridLayout;

class WXDLLIMPEXP_CORE wxRadioBox : public wxControl, public wxRadioBoxBase
{
public:
    wxRadioBox();

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = NULL,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr);

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);

    using wxWindowBase::Show;
    using wxWindowBase::Enable;
    using wxRadioBoxBase::GetDefaultBorder;

    virtual bool Enable(unsigned int n, bool enable = true) wxOVERRIDE;
    virtual bool Enable(bool enable = true) wxOVERRIDE;
    virtual bool Show(unsigned int n, bool show = true) wxOVERRIDE;
    virtual bool Show(bool show = true) wxOVERRIDE;
    virtual bool IsItemEnabled(unsigned int n) const wxOVERRIDE;
    virtual bool IsItemShown(unsigned int n) const wxOVERRIDE;

    virtual unsigned int GetCount() const wxOVERRIDE;
    virtual wxString GetString(unsigned int n) const wxOVERRIDE;
    virtual void SetString(unsigned int n, const wxString& s) wxOVERRIDE;

    virtual void SetSelection(int n) wxOVERRIDE;
    virtual int GetSelection() const wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

private:
    // The 'visual' group box:
    QGroupBox *m_qtGroupBox;

    // Handles the mutual exclusion of buttons:
    QButtonGroup *m_qtButtonGroup;

    // Autofit layout for buttons (either vert. or horiz.):
    QGridLayout *m_qtGridLayout;

    wxDECLARE_DYNAMIC_CLASS(wxRadioBox);
};

#endif // _WX_QT_RADIOBOX_H_
