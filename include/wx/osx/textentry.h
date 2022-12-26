/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/textentry.h
// Purpose:     wxTextEntry class
// Author:      Stefan Csomor
// Modified by: Kevin Ollivier
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_TEXTENTRY_H_
#define _WX_OSX_TEXTENTRY_H_

#if wxUSE_SYSTEM_OPTIONS
    // set this to 'true' if you want to use the 'classic' MLTE-based implementation
    // instead of the HIView-based implementation in 10.3 and upwards, the former
    // has more features (backgrounds etc.), but may show redraw artefacts and other
    // problems depending on your usage; hence, the default is 'false'.
    #define wxMAC_TEXTCONTROL_USE_MLTE wxT("mac.textcontrol-use-mlte")
    // set this to 'true' if you want editable text controls to have spell checking turned
    // on by default, you can change this setting individually on a control using MacCheckSpelling
    #define wxMAC_TEXTCONTROL_USE_SPELL_CHECKER wxT("mac.textcontrol-use-spell-checker")
#endif

#include "wx/control.h"

// forward decl for wxListWidgetImpl implementation type.
class WXDLLIMPEXP_FWD_CORE wxTextWidgetImpl;

class WXDLLIMPEXP_CORE wxTextEntry: public wxTextEntryBase
{

public:
    wxTextEntry();
    virtual ~wxTextEntry();

    virtual bool IsEditable() const override;

    // If the return values from and to are the same, there is no selection.
    virtual void GetSelection(long* from, long* to) const override;

    // operations
    // ----------

    // editing
    virtual void Clear() override;
    virtual void Remove(long from, long to) override;

    // set the max number of characters which may be entered
    // in a single line text control
    virtual void SetMaxLength(unsigned long len) override;

    virtual void ForceUpper() override;

    // writing text inserts it at the current position;
    // appending always inserts it at the end
    virtual void WriteText(const wxString& text) override;

    // Clipboard operations
    virtual void Copy() override;
    virtual void Cut() override;
    virtual void Paste() override;

    virtual bool CanCopy() const override;
    virtual bool CanCut() const override;
    virtual bool CanPaste() const override;

    // Undo/redo
    virtual void Undo() override;
    virtual void Redo() override;

    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;

    // Insertion point
    virtual void SetInsertionPoint(long pos) override;
    virtual void SetInsertionPointEnd() override;
    virtual long GetInsertionPoint() const override;
    virtual wxTextPos GetLastPosition() const override;

    virtual void SetSelection(long from, long to) override;
    virtual void SetEditable(bool editable) override;

    virtual bool SendMaxLenEvent();

    // set the grayed out hint text
    virtual bool SetHint(const wxString& hint) override;
    virtual wxString GetHint() const override;

    // Implementation
    // --------------

    virtual wxTextWidgetImpl * GetTextPeer() const;
    wxTextCompleter *OSXGetCompleter() const { return m_completer; }

protected:

    virtual wxString DoGetValue() const override;

    virtual bool DoAutoCompleteStrings(const wxArrayString& choices) override;
    virtual bool DoAutoCompleteCustom(wxTextCompleter *completer) override;

    // The object providing auto-completions or nullptr if none.
    wxTextCompleter *m_completer;

    bool  m_editable;

  // need to make this public because of the current implementation via callbacks
    unsigned long  m_maxLength;

private:
    wxString m_hintString;
};

#endif // _WX_OSX_TEXTENTRY_H_
