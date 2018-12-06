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

    virtual bool IsSelected(int n) const;
    virtual int GetSelections(wxArrayInt& aSelections) const;
    
    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);
    
    virtual int GetSelection() const;

    virtual QWidget *GetHandle() const;

    void QtSendEvent(wxEventType evtType, const QModelIndex &index, bool selected);

protected:
    virtual void DoSetFirstItem(int n);

    virtual void DoSetSelection(int n, bool select);
    
    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type);
    virtual int DoInsertOneItem(const wxString& item, unsigned int pos);
    
    virtual void DoSetItemClientData(unsigned int n, void *clientData);
    virtual void *DoGetItemClientData(unsigned int n) const;
    
    virtual void DoClear();
    virtual void DoDeleteOneItem(unsigned int pos);

    virtual QScrollArea *QtGetScrollBarsContainer() const;

#if wxUSE_CHECKLISTBOX
    bool       m_hasCheckBoxes;
#endif // wxUSE_CHECKLISTBOX

    QListWidget *m_qtListWidget;

private:
    virtual void Init(); //common construction

    void UnSelectAll();

    wxDECLARE_DYNAMIC_CLASS(wxListBox);
};

#endif // _WX_QT_LISTBOX_H_
