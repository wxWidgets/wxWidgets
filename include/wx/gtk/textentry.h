///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/textentry.h
// Purpose:     wxGTK-specific wxTextEntry implementation
// Author:      Vadim Zeitlin
// Created:     2007-09-24
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TEXTENTRY_H_
#define _WX_GTK_TEXTENTRY_H_

typedef struct _GdkEventKey GdkEventKey;
typedef struct _GtkEditable GtkEditable;
typedef struct _GtkEntry GtkEntry;

class wxTextAutoCompleteData; // private class used only by wxTextEntry itself
class wxTextCoalesceData;     // another private class

// ----------------------------------------------------------------------------
// wxTextEntry: roughly corresponds to GtkEditable
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

    virtual void SetSelection(long from, long to) override;
    virtual void GetSelection(long *from, long *to) const override;

    virtual bool IsEditable() const override;
    virtual void SetEditable(bool editable) override;

    virtual void SetMaxLength(unsigned long len) override;
    virtual void ForceUpper() override;

#ifdef __WXGTK3__
    virtual bool SetHint(const wxString& hint) override;
    virtual wxString GetHint() const override;
#endif

    // implementation only from now on
    void SendMaxLenEvent();
    bool GTKEntryOnInsertText(const char* text);
    bool GTKIsUpperCase() const { return m_isUpperCase; }

    // Called from "changed" signal handler (or, possibly, slightly later, when
    // coalescing several "changed" signals into a single event) for GtkEntry.
    //
    // By default just generates a wxEVT_TEXT, but overridden to do more things
    // in wxTextCtrl.
    virtual void GTKOnTextChanged() { SendTextUpdatedEvent(); }

    // Helper functions only used internally.
    wxTextCoalesceData* GTKGetCoalesceData() const { return m_coalesceData; }

protected:
    // This method must be called from the derived class Create() to connect
    // the handlers for the clipboard (cut/copy/paste) events.
    void GTKConnectClipboardSignals(GtkWidget* entry);

    // And this one to connect "insert-text" signal.
    void GTKConnectInsertTextSignal(GtkEntry* entry);

    // Finally this one connects to the "changed" signal on the object returned
    // by GetTextObject().
    void GTKConnectChangedSignal();


    virtual void DoSetValue(const wxString& value, int flags) override;
    virtual wxString DoGetValue() const override;

    // margins functions
    virtual bool DoSetMargins(const wxPoint& pt) override;
    virtual wxPoint DoGetMargins() const override;

    virtual bool DoAutoCompleteStrings(const wxArrayString& choices) override;
    virtual bool DoAutoCompleteCustom(wxTextCompleter *completer) override;

    // Call this from the overridden wxWindow::GTKIMFilterKeypress() to use
    // GtkEntry IM context.
    int GTKEntryIMFilterKeypress(GdkEventKey* event) const;

    // If GTKEntryIMFilterKeypress() is not called (as multiline wxTextCtrl
    // uses its own IM), call this method instead to still notify wxTextEntry
    // about the key press events in the given widget.
    void GTKEntryOnKeypress(GtkWidget* widget) const;


    static int GTKGetEntryTextLength(GtkEntry* entry);

    // Block/unblock the corresponding GTK signal.
    //
    // Note that we make it protected in wxGTK as it is called from wxComboBox
    // currently.
    virtual void EnableTextChangedEvents(bool enable) override;

    // Helper for wxTE_PROCESS_ENTER handling: activates the default button in
    // the dialog containing this control if any.
    bool ClickDefaultButtonIfPossible();

private:
    // implement this to return the associated GtkEntry or another widget
    // implementing GtkEditable
    virtual GtkEditable *GetEditable() const = 0;

    // implement this to return the associated GtkEntry
    virtual GtkEntry *GetEntry() const = 0;

    // This one exists in order to be overridden by wxTextCtrl which uses
    // either GtkEditable or GtkTextBuffer depending on whether it is single-
    // or multi-line.
    virtual void *GetTextObject() const { return GetEntry(); }


    // Various auto-completion-related stuff, only used if any of AutoComplete()
    // methods are called.
    wxTextAutoCompleteData *m_autoCompleteData;

    // It needs to call our GetEntry() method.
    friend class wxTextAutoCompleteData;

    // Data used for coalescing "changed" events resulting from a single user
    // action.
    mutable wxTextCoalesceData* m_coalesceData;

    bool m_isUpperCase;
};

// We don't need the generic version.
#define wxHAS_NATIVE_TEXT_FORCEUPPER

#endif // _WX_GTK_TEXTENTRY_H_

