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
    virtual void WriteText(const wxString& text) wxOVERRIDE;
    virtual void Remove(long from, long to) wxOVERRIDE;

    virtual void Copy() wxOVERRIDE;
    virtual void Cut() wxOVERRIDE;
    virtual void Paste() wxOVERRIDE;

    virtual void Undo() wxOVERRIDE;
    virtual void Redo() wxOVERRIDE;
    virtual bool CanUndo() const wxOVERRIDE;
    virtual bool CanRedo() const wxOVERRIDE;

    virtual void SetInsertionPoint(long pos) wxOVERRIDE;
    virtual long GetInsertionPoint() const wxOVERRIDE;
    virtual long GetLastPosition() const wxOVERRIDE;

    virtual void SetSelection(long from, long to) wxOVERRIDE
        { DoSetSelection(from, to); }
    virtual void GetSelection(long *from, long *to) const wxOVERRIDE;

    virtual bool IsEditable() const wxOVERRIDE;
    virtual void SetEditable(bool editable) wxOVERRIDE;

    virtual void SetMaxLength(unsigned long len) wxOVERRIDE;
    virtual void ForceUpper() wxOVERRIDE;

#if wxUSE_UXTHEME
    virtual bool SetHint(const wxString& hint) wxOVERRIDE;
    virtual wxString GetHint() const wxOVERRIDE;
#endif // wxUSE_UXTHEME

protected:
    virtual wxString DoGetValue() const wxOVERRIDE;

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
    virtual bool DoSetMargins(const wxPoint& pt) wxOVERRIDE;
    virtual wxPoint DoGetMargins() const wxOVERRIDE;

    // auto-completion uses COM under Windows so they won't work without
    // wxUSE_OLE as OleInitialize() is not called then
#if wxUSE_OLE
    virtual bool DoAutoCompleteStrings(const wxArrayString& choices) wxOVERRIDE;
#if wxUSE_DYNLIB_CLASS
    virtual bool DoAutoCompleteFileNames(int flags) wxOVERRIDE;
#endif // wxUSE_DYNLIB_CLASS
    virtual bool DoAutoCompleteCustom(wxTextCompleter *completer) wxOVERRIDE;
#endif // wxUSE_OLE

    // Returns true if this control uses standard file names completion.
    bool MSWUsesStandardAutoComplete() const;

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
    // checking if m_autoCompleteData is NULL, see the code for more details.
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

