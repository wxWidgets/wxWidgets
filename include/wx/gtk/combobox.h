/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/combobox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKCOMBOBOXH__
#define __GTKCOMBOBOXH__

#include "wx/defs.h"

#if wxUSE_COMBOBOX

#include "wx/object.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxComboBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern WXDLLEXPORT_DATA(const wxChar) wxComboBoxNameStr[];
extern WXDLLIMPEXP_BASE const wxChar* wxEmptyString;

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxComboBox : public wxControl, public wxComboBoxBase
{
public:
    inline wxComboBox() {}
    inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (const wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }
    inline wxComboBox(wxWindow *parent, wxWindowID id,
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

    ~wxComboBox();

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
    wxString GetStringSelection() const; // not a virtual in parent class

    // From wxItemContainer:
    virtual void Clear();
    virtual void Delete(unsigned int n);

    // From wxBomboBoxBase:
    virtual wxString GetValue() const;
    virtual void SetValue(const wxString& value);
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint( long pos );
    virtual long GetInsertionPoint() const;
    virtual wxTextPos GetLastPosition() const;
    virtual void Replace( long from, long to, const wxString& value );
    virtual void SetSelection( long from, long to );
    virtual void SetEditable( bool editable );
    virtual void SetInsertionPointEnd() { SetInsertionPoint( -1 ); }
    virtual void Remove(long from, long to) { Replace(from, to, wxEmptyString); }
    virtual bool IsEditable() const;
    virtual void Undo();
    virtual void Redo();
    virtual void SelectAll();
    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;
    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    // implementation
    bool HasSelection() const;
    void GetSelection( long* from, long* to ) const;
    int GetCurrentSelection() const;

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

    bool     m_ignoreNextUpdate:1;
    wxList   m_clientDataList;
    wxList   m_clientObjectList;
    int      m_prevSelection;

    void DisableEvents();
    void EnableEvents();
    GtkWidget* GetConnectWidget();

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

protected:
    // From wxWindowGTK:
    virtual void DoApplyWidgetStyle(GtkRcStyle *style);
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

    // From wxItemContainer:
    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, unsigned int pos);

    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;

    // From wxControl:
    virtual wxSize DoGetBestSize() const;

    // Widgets that use the style->base colour for the BG colour should
    // override this and return true.
    virtual bool UseGTKStyleBase() const { return true; }

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxComboBox)
    DECLARE_EVENT_TABLE()
};

#endif

#endif

  // __GTKCOMBOBOXH__
