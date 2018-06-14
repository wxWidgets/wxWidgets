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

    virtual void SetSelection(int n) wxOVERRIDE { wxChoice::SetSelection(n); }
    virtual void SetSelection(long from, long to) wxOVERRIDE;

    virtual int GetSelection() const wxOVERRIDE { return wxChoice::GetSelection(); }
    virtual void GetSelection(long *from, long *to) const wxOVERRIDE;

    virtual wxString GetStringSelection() const wxOVERRIDE
    {
        return wxItemContainer::GetStringSelection();
    }

    virtual void Clear() wxOVERRIDE
    {
        wxTextEntry::Clear();
        wxItemContainer::Clear();
    }

    // See wxComboBoxBase discussion of IsEmpty().
    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    virtual void Popup();
    virtual void Dismiss();

protected:

    // From wxTextEntry:
    virtual wxString DoGetValue() const wxOVERRIDE;

private:

    // From wxTextEntry:
    virtual wxWindow *GetEditableWindow() wxOVERRIDE { return this; }

    wxDECLARE_DYNAMIC_CLASS(wxComboBox);
};

#if wxUSE_VALIDATORS

template<>
struct wxDataTransfer<wxComboBox>
{
    static bool To(wxComboBox* combo, int* data)
    {
        combo->SetSelection(*data);
        return true;
    }

    static bool To(wxComboBox* combo, wxString* data)
    {
        if ( combo->FindString(*data) != wxNOT_FOUND )
        {
            combo->SetStringSelection(*data);
        }
        else if ( (combo->GetWindowStyle() & wxCB_READONLY) == 0 )
        {
            combo->SetValue(*data);
        }
        else
        {
            return false;
        }

        return true;
    }

    static bool From(wxComboBox* combo, int* data)
    {
        *data = combo->GetSelection();
        return true;
    }

    static bool From(wxComboBox* combo, wxString* data)
    {
        if ( combo->GetWindowStyle() & wxCB_READONLY )
            *data = combo->GetStringSelection();
        else
            *data = combo->GetValue();

        return true;
    }
};

#endif // wxUSE_VALIDATORS

#endif // _WX_QT_COMBOBOX_H_
