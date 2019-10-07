/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/listbox.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_LISTBOX_H_
#define _WX_QT_LISTBOX_H_

class QListWidget;
class QModelIndex;
class QScrollArea;

class WXDLLIMPEXP_CORE wxListBox : public wxListBoxBase
{
public:
    wxListBox();
    wxListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr);

    wxListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr);

    virtual ~wxListBox();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);

    virtual bool IsSelected(int n) const wxOVERRIDE;
    virtual int GetSelections(wxArrayInt& aSelections) const wxOVERRIDE;

    virtual unsigned int GetCount() const wxOVERRIDE;
    virtual wxString GetString(unsigned int n) const wxOVERRIDE;
    virtual void SetString(unsigned int n, const wxString& s) wxOVERRIDE;

    virtual int GetSelection() const wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

    void QtSendEvent(wxEventType evtType, int rowIndex, bool selected);

protected:
    virtual void DoSetFirstItem(int n) wxOVERRIDE;

    virtual void DoSetSelection(int n, bool select) wxOVERRIDE;

    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type) wxOVERRIDE;
    virtual int DoInsertOneItem(const wxString& item, unsigned int pos) wxOVERRIDE;

    virtual void DoSetItemClientData(unsigned int n, void *clientData) wxOVERRIDE;
    virtual void *DoGetItemClientData(unsigned int n) const wxOVERRIDE;

    virtual void DoClear() wxOVERRIDE;
    virtual void DoDeleteOneItem(unsigned int pos) wxOVERRIDE;

    virtual QScrollArea *QtGetScrollBarsContainer() const wxOVERRIDE;

#if wxUSE_CHECKLISTBOX
    bool       m_hasCheckBoxes;
#endif // wxUSE_CHECKLISTBOX

    QListWidget *m_qtListWidget;

private:
    virtual void Init(); //common construction

    // Common part of both Create() overloads.
    void DoCreate(wxWindow* parent, long style);

    void UnSelectAll();

    wxDECLARE_DYNAMIC_CLASS(wxListBox);
};

#endif // _WX_QT_LISTBOX_H_
