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
            int n = 0, const wxString choices[] = nullptr,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxListBoxNameStr));

    wxListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxListBoxNameStr));

    virtual ~wxListBox();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxListBoxNameStr));

    virtual bool IsSelected(int n) const override;
    virtual int GetSelections(wxArrayInt& aSelections) const override;

    virtual unsigned int GetCount() const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& s) override;

    virtual int GetSelection() const override;

    virtual void EnsureVisible(int item) override;

    virtual int GetTopItem() const override;

    virtual int GetCountPerPage() const override;

    virtual QWidget *GetHandle() const override;

    void QtSendEvent(wxEventType evtType, int rowIndex, bool selected);

protected:
    virtual void DoSetFirstItem(int n) override;

    virtual void DoSetSelection(int n, bool select) override;

    virtual int DoInsertItems(const wxArrayStringsAdapter & items,
                              unsigned int pos,
                              void **clientData,
                              wxClientDataType type) override;
    virtual int DoInsertOneItem(const wxString& item, unsigned int pos) override;

    virtual void DoSetItemClientData(unsigned int n, void *clientData) override;
    virtual void *DoGetItemClientData(unsigned int n) const override;

    virtual void DoClear() override;
    virtual void DoDeleteOneItem(unsigned int pos) override;

    virtual int DoListHitTest(const wxPoint& point) const override;

    virtual QScrollArea *QtGetScrollBarsContainer() const override;

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
