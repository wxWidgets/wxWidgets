/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/combobox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_COMBOBOX_H_
#define _WX_GTK_COMBOBOX_H_

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxComboBox : public wxControl,
                                    public wxComboBoxBase
{
public:
    wxComboBox() { m_strings = NULL; }
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }

    wxComboBox(wxWindow *parent, wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
    {
        Create(parent, id, value, pos, size, choices, style, validator, name);
    }

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

    // From wxItemContainerImmutable:
    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString &text);
    virtual int FindString(const wxString& s, bool bCase = false) const;
    virtual void SetSelection(int n);
    virtual int GetSelection() const;

    // from wxTextEntry: we need to override them to avoid virtual function
    // hiding
    virtual void SetSelection(long from, long to)
    {
        wxTextEntry::SetSelection(from, to);
    }

    virtual void GetSelection(long *from, long *to) const
    {
        return wxTextEntry::GetSelection(from, to);
    }

    virtual wxString GetStringSelection() const
    {
        return wxItemContainer::GetStringSelection();
    }

    // From wxComboBoxBase:
    virtual int GetCurrentSelection() const;

    virtual void SetFocus();

    void OnSize( wxSizeEvent &event );
    void OnChar( wxKeyEvent &event );

    // Standard event handling
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    void OnUpdateCut(wxUpdateUIEvent& event);
    void OnUpdateCopy(wxUpdateUIEvent& event);
    void OnUpdatePaste(wxUpdateUIEvent& event);
    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnUpdateDelete(wxUpdateUIEvent& event);
    void OnUpdateSelectAll(wxUpdateUIEvent& event);

    bool           m_ignoreNextUpdate:1;
    wxArrayPtrVoid m_clientData;
    int            m_prevSelection;

    void DisableEvents();
    void EnableEvents();
    GtkWidget* GetConnectWidget();

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    virtual bool IsSorted() const { return HasFlag(wxCB_SORT); }

protected:
    // From wxWindowGTK:
    virtual void DoApplyWidgetStyle(GtkRcStyle *style);
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

    // From wxItemContainer:
    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type);
    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoClear();
    virtual void DoDeleteOneItem(unsigned int n);

    // From wxControl:
    virtual wxSize DoGetBestSize() const;

    // Widgets that use the style->base colour for the BG colour should
    // override this and return true.
    virtual bool UseGTKStyleBase() const { return true; }

private:
    // From wxTextEntry:
    virtual const wxWindow *GetEditableWindow() const { return this; }
    virtual GtkEditable *GetEditable() const;
    virtual void EnableTextChangedEvents(bool enable)
    {
        if ( enable )
            EnableEvents();
        else
            DisableEvents();
    }

    // this array is only used for controls with wxCB_SORT style, so only
    // allocate it if it's needed (hence using pointer)
    wxSortedArrayString *m_strings;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxComboBox)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GTK_COMBOBOX_H_
