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
               int n = 0, const wxString choices[] = NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);

    wxComboBox(wxWindow *parent, wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = (const wxString *) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual void SetSelection(int n) wxOVERRIDE;
    virtual void SetSelection(long from, long to) wxOVERRIDE;

    virtual int GetSelection() const wxOVERRIDE { return wxChoice::GetSelection(); }
    virtual void GetSelection(long *from, long *to) const wxOVERRIDE;

    virtual wxString GetStringSelection() const wxOVERRIDE
    {
        return wxItemContainer::GetStringSelection();
    }

    virtual void Clear() wxOVERRIDE;

    // See wxComboBoxBase discussion of IsEmpty().
    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    virtual void SetValue(const wxString& value) wxOVERRIDE;
    virtual void ChangeValue(const wxString& value) wxOVERRIDE;
    virtual void AppendText(const wxString &value) wxOVERRIDE;
    virtual void Replace(long from, long to, const wxString &value) wxOVERRIDE;
    virtual void WriteText(const wxString &value) wxOVERRIDE;
    virtual void SetInsertionPoint(long insertion) wxOVERRIDE;
    virtual long GetInsertionPoint() const wxOVERRIDE;

    virtual void Popup();
    virtual void Dismiss();

    virtual bool QtHandleFocusEvent(QWidget *handler, QFocusEvent *event) wxOVERRIDE;
protected:

    // From wxTextEntry:
    virtual wxString DoGetValue() const wxOVERRIDE;

private:
    void SetActualValue(const wxString& value);
    bool IsReadOnly() const;

    // From wxTextEntry:
    virtual wxWindow *GetEditableWindow() wxOVERRIDE { return this; }

    wxDECLARE_DYNAMIC_CLASS(wxComboBox);
};

#endif // _WX_QT_COMBOBOX_H_
