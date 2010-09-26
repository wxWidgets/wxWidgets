/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/combobox.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COMBOBOX_H_
#define _WX_QT_COMBOBOX_H_

#include "wx/qt/pointer_qt.h"
#include "wx/qt/combobox_qt.h"

class WXDLLIMPEXP_CORE wxComboBox : public wxControl, public wxComboBoxBase
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

    virtual ~wxComboBox();

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

    virtual void SetSelection(int n);
    virtual void SetSelection(long from, long to);

    virtual int GetSelection() const;
    virtual void GetSelection(long *from, long *to) const;
    virtual wxString GetStringSelection() const;

    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);
    
    virtual QComboBox *GetHandle() const;

protected:
    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type);
    
    virtual void DoSetItemClientData(unsigned int n, void *clientData);
    virtual void *DoGetItemClientData(unsigned int n) const;
    
    virtual void DoClear();
    virtual void DoDeleteOneItem(unsigned int pos);

private:
    wxQtPointer< wxQtComboBox > m_qtComboBox;

    DECLARE_DYNAMIC_CLASS(wxComboBox)
};

#endif // _WX_QT_COMBOBOX_H_
