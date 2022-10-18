///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/textentry.h
// Purpose:     wxMSW-specific wxTextEntry implementation
// Author:      Vadim Zeitlin
// Created:     2007-09-26
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_TEXTENTRY_H_
#define _WX_MSW_TEXTENTRY_H_

class wxTextAutoCompleteData; // private class used only by wxTextEntry itself

// ----------------------------------------------------------------------------
// wxTextEntry: common part of wxComboBox and (single line) wxTextCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextEntry : public wxTextEntryBase
{
public:
    wxTextEntry();
    virtual ~wxTextEntry();

    // implement wxTextEntryBase pure virtual methods
    virtual void WriteText(const wxString& text) override;
    virtual void Remove(long from, long to) override;

    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;

    virtual void Undo() override;
    virtual void Redo() override;
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    virtual void SetInsertionPoint(long pos) override;
    virtual long GetInsertionPoint() const override;
    virtual long GetLastPosition() const override;

    virtual void SetSelection(long from, long to) override
        { DoSetSelection(from, to); }
    virtual void GetSelection(long *from, long *to) const override;

    virtual bool IsEditable() const override;
    virtual void SetEditable(bool editable) override;

    virtual void SetMaxLength(unsigned long len) override;
    virtual void ForceUpper() override;

#if wxUSE_UXTHEME
    virtual bool SetHint(const wxString& hint) override;
    virtual wxString GetHint() const override;
#endif // wxUSE_UXTHEME

protected:
    virtual wxString DoGetValue() const override;

    // this is really a hook for multiline text controls as the single line
    // ones don't need to ever scroll to show the selection but having it here
    // allows us to put Remove() in the base class
    enum
    {
        SetSel_NoScroll = 0,    // don't do anything special
        SetSel_Scroll = 1       // default: scroll to make the selection visible
    };
    virtual void DoSetSelection(long from, long to, int flags = SetSel_Scroll);

    // margins functions
    virtual bool DoSetMargins(const wxPoint& pt) override;
    virtual wxPoint DoGetMargins() const override;

    // auto-completion uses COM under Windows so they won't work without
    // wxUSE_OLE as OleInitialize() is not called then
#if wxUSE_OLE
    virtual bool DoAutoCompleteStrings(const wxArrayString& choices) override;
#if wxUSE_DYNLIB_CLASS
    virtual bool DoAutoCompleteFileNames(int flags) override;
#endif // wxUSE_DYNLIB_CLASS
    virtual bool DoAutoCompleteCustom(wxTextCompleter *completer) override;
#endif // wxUSE_OLE

    // Returns true if this control uses standard file names completion.
    bool MSWUsesStandardAutoComplete() const;

    // Returns false if this message shouldn't be preprocessed, but is always
    // handled by the EDIT control represented by this object itself.
    bool MSWShouldPreProcessMessage(WXMSG* msg) const;

    // Helper for wxTE_PROCESS_ENTER handling: activates the default button in
    // the dialog containing this control if any.
    bool ClickDefaultButtonIfPossible();

private:
    // implement this to return the HWND of the EDIT control
    virtual WXHWND GetEditHWND() const = 0;

#if wxUSE_OLE
    // This method is called to process special keys such as Return and Tab
    // before they're consumed by the auto-completer. Notice that it is only
    // called if we do need to process the key, i.e. if the corresponding
    // wxTE_PROCESS_XXX style is set in the associated object.
    //
    // It is not pure virtual because it won't get called if the derived class
    // doesn't use auto-completer, but it does need to be overridden if it can
    // be called and the default implementation asserts if this is not the case.
    virtual void MSWProcessSpecialKey(wxKeyEvent& event);

    // Check if we really have auto-complete data. This is not the same as just
    // checking if m_autoCompleteData is null, see the code for more details.
    bool MSWHasAutoCompleteData() const;

    // Check that we have auto-complete data, creating it if necessary. Returns
    // false if creating it failed.
    bool MSWEnsureHasAutoCompleteData();

    // Various auto-completion-related stuff, only used if any of AutoComplete()
    // methods are called. Use the function above to access it.
    wxTextAutoCompleteData *m_autoCompleteData;

    // It needs to call our GetEditableWindow() and GetEditHWND() methods.
    friend class wxTextAutoCompleteData;
#endif // wxUSE_OLE
};

// We don't need the generic version.
#define wxHAS_NATIVE_TEXT_FORCEUPPER

#endif // _WX_MSW_TEXTENTRY_H_

