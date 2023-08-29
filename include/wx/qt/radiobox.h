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
               int n = 0, const wxString choices[] = nullptr,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxRadioBoxNameStr));

    using wxWindowBase::Show;
    using wxWindowBase::Enable;
    using wxRadioBoxBase::GetDefaultBorder;

    virtual bool Enable(unsigned int n, bool enable = true) override;
    virtual bool Enable(bool enable = true) override;
    virtual bool Show(unsigned int n, bool show = true) override;
    virtual bool Show(bool show = true) override;
    virtual bool IsItemEnabled(unsigned int n) const override;
    virtual bool IsItemShown(unsigned int n) const override;

    virtual unsigned int GetCount() const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& s) override;

    virtual void SetSelection(int n) override;
    virtual int GetSelection() const override;

    virtual QWidget *GetHandle() const override;

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
