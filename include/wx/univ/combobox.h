///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/combobox.h
// Purpose:     the universal combobox
// Author:      Vadim Zeitlin
// Created:     30.08.00
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_UNIV_COMBOBOX_H_
#define _WX_UNIV_COMBOBOX_H_

#include "wx/combo.h"

class WXDLLIMPEXP_FWD_CORE wxListBox;

// ----------------------------------------------------------------------------
// NB: some actions supported by this control are in wx/generic/combo.h
// ----------------------------------------------------------------------------

// choose the next/prev/specified (by numArg) item
#define wxACTION_COMBOBOX_SELECT_NEXT wxT("next")
#define wxACTION_COMBOBOX_SELECT_PREV wxT("prev")
#define wxACTION_COMBOBOX_SELECT      wxT("select")


// ----------------------------------------------------------------------------
// wxComboBox: a combination of text control and a listbox
// ----------------------------------------------------------------------------

// NB: Normally we'd like wxComboBox to inherit from wxComboBoxBase, but here
//     we can't really do that since both wxComboBoxBase and wxComboCtrl inherit
//     from wxTextCtrl.
class WXDLLIMPEXP_CORE wxComboBox :
    public wxWindowWithItems<wxComboCtrl, wxItemContainer>
{
public:
    // ctors and such
    wxComboBox() { Init(); }

    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = nullptr,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr))
    {
        Init();

        (void)Create(parent, id, value, pos, size, n, choices,
                     style, validator, name);
    }
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = nullptr,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    virtual ~wxComboBox();

    // the wxUniversal-specific methods
    // --------------------------------

    // implement the combobox interface

    // wxTextCtrl methods
    virtual wxString GetValue() const override { return DoGetValue(); }
    virtual void SetValue(const wxString& value) override;
    virtual void WriteText(const wxString& value) override;
    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;
    virtual void SetInsertionPoint(long pos) override;
    virtual void SetInsertionPointEnd() override;
    virtual long GetInsertionPoint() const override;
    virtual wxTextPos GetLastPosition() const override;
    virtual void Replace(long from, long to, const wxString& value) override;
    virtual void Remove(long from, long to) override;
    virtual void SetSelection(long from, long to) override;
    virtual void GetSelection(long *from, long *to) const override;
    virtual void SetEditable(bool editable) override;
    virtual bool IsEditable() const override;

    virtual void Undo() override;
    virtual void Redo() override;
    virtual void SelectAll() override;

    virtual bool CanCopy() const override;
    virtual bool CanCut() const override;
    virtual bool CanPaste() const override;
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    // override these methods to disambiguate between two base classes versions
    virtual void Clear() override
    {
        wxItemContainer::Clear();
    }

    // See wxComboBoxBase discussion of IsEmpty().
    bool IsListEmpty() const { return wxItemContainer::IsEmpty(); }
    bool IsTextEmpty() const { return wxTextEntry::IsEmpty(); }

    // wxControlWithItems methods
    virtual void DoClear() override;
    virtual void DoDeleteOneItem(unsigned int n) override;
    virtual unsigned int GetCount() const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual void SetString(unsigned int n, const wxString& s) override;
    virtual int FindString(const wxString& s, bool bCase = false) const override;
    virtual void SetSelection(int n) override;
    virtual int GetSelection() const override;
    virtual wxString GetStringSelection() const override;

    // we have our own input handler and our own actions
    // (but wxComboCtrl already handled Popup/Dismiss)
    /*
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0l,
                               const wxString& strArg = wxEmptyString);
    */

    static wxInputHandler *GetStdInputHandler(wxInputHandler *handlerDef);
    virtual wxInputHandler *DoGetStdInputHandler(wxInputHandler *handlerDef) override
    {
        return GetStdInputHandler(handlerDef);
    }

    // we delegate our client data handling to wxListBox which we use for the
    // items, so override this and other methods dealing with the client data
    virtual wxClientDataType GetClientDataType() const override;
    virtual void SetClientDataType(wxClientDataType clientDataItemsType) override;

    virtual const wxTextEntry* WXGetTextEntry() const override { return this; }

protected:
    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type) override;

    virtual void DoSetItemClientData(unsigned int n, void* clientData) override;
    virtual void* DoGetItemClientData(unsigned int n) const override;


    // common part of all ctors
    void Init();

    // get the associated listbox
    wxListBox *GetLBox() const { return m_lbox; }

private:
    // implement wxTextEntry pure virtual method
    virtual wxWindow *GetEditableWindow() override { return this; }

    // the popup listbox
    wxListBox *m_lbox;

    //wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxComboBox);
};

#endif // _WX_UNIV_COMBOBOX_H_
