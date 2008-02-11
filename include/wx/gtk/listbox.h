/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/listbox.h
// Purpose:     wxListBox class declaration
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKLISTBOXH__
#define __GTKLISTBOXH__

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxListBox : public wxListBoxBase
{
public:
    // ctors and such
    wxListBox()
    {
        Init();
    }
    wxListBox( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = (const wxString *) NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr )
    {
        Init();
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    wxListBox( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr )
    {
        Init();
        Create(parent, id, pos, size, choices, style, validator, name);
    }
    virtual ~wxListBox();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = (const wxString *) NULL,
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

    // implement base class pure virtuals
    virtual void Clear();
    virtual void Delete(unsigned int n);

    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);
    virtual int FindString(const wxString& s, bool bCase = false) const;

    virtual bool IsSelected(int n) const;
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

    virtual void EnsureVisible(int n);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation from now on

    GtkWidget *GetConnectWidget();

#if wxUSE_TOOLTIPS
    void ApplyToolTip( GtkTooltips *tips, const wxChar *tip );
#endif // wxUSE_TOOLTIPS

    struct _GtkTreeView   *m_treeview;
    struct _GtkListStore  *m_liststore;

#if wxUSE_CHECKLISTBOX
    bool       m_hasCheckBoxes;
#endif // wxUSE_CHECKLISTBOX

    bool       m_blockEvent;

    struct _GtkTreeEntry* GtkGetEntry(int pos) const;
    void GtkInsertItems(const wxArrayString& items,
                        void** clientData, unsigned int pos);
    void GtkDeselectAll();
    void GtkSetSelection(int n, const bool select, const bool blockEvent);

protected:
    virtual wxSize DoGetBestSize() const;
    virtual void DoApplyWidgetStyle(GtkRcStyle *style);
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

    virtual void DoSetSelection(int n, bool select);
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, unsigned int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoSetFirstItem(int n);
    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;
    virtual int DoListHitTest(const wxPoint& point) const;

private:
    void Init(); //common construction

    // common part of DoSetFirstItem() and EnsureVisible()
    void DoScrollToCell(int n, float alignY, float alignX);

    DECLARE_DYNAMIC_CLASS(wxListBox)
};

#endif // __GTKLISTBOXH__
