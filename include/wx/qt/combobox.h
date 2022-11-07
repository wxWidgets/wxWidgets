/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/combobox.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COMBOBOX_H_
#define _WX_QT_COMBOBOX_H_

#include "wx/choice.h"
class QComboBox;

class WXDLLIMPEXP_CORE wxComboBox : public wxChoice, public wxTextEntry
{
public:
    wxComboBox();

    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = nullptr,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    wxComboBox(wxWindow *parent, wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    virtual void SetSelection(int n) override;
    virtual void SetSelection(long from, long to) override;

    virtual int GetSelection() const override { return wxChoice::GetSelection(); }
    virtual void GetSelection(long *from, long *to) const override;

    virtual wxString GetStringSelection() const override
    {
        return wxItemContainer::GetStringSelection();
    }

    virtual void Clear() override;

    // See wxComboBoxBase discussion of IsEmpty().
    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    virtual void SetValue(const wxString& value) override;
    virtual void ChangeValue(const wxString& value) override;
    virtual void AppendText(const wxString &value) override;
    virtual void Replace(long from, long to, const wxString &value) override;
    virtual void WriteText(const wxString &value) override;
    virtual void SetInsertionPoint(long insertion) override;
    virtual long GetInsertionPoint() const override;
    virtual bool IsEditable() const override;
    virtual void SetEditable(bool editable) override;

    virtual void Popup();
    virtual void Dismiss();

    virtual const wxTextEntry* WXGetTextEntry() const override { return this; }

    virtual bool QtHandleFocusEvent(QWidget *handler, QFocusEvent *event) override;
protected:

    // From wxTextEntry:
    virtual wxString DoGetValue() const override;

private:
    void SetActualValue(const wxString& value);
    bool IsReadOnly() const;

    // From wxTextEntry:
    virtual wxWindow *GetEditableWindow() override { return this; }

    wxDECLARE_DYNAMIC_CLASS(wxComboBox);
};

#endif // _WX_QT_COMBOBOX_H_
