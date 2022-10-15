/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cocoa/private/textimpl.h
// Purpose:     textcontrol implementation classes that have to be exposed
// Author:      Stefan Csomor
// Modified by:
// Created:     03/02/99
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_COCOA_PRIVATE_TEXTIMPL_H_
#define _WX_OSX_COCOA_PRIVATE_TEXTIMPL_H_

#include "wx/combobox.h"
#include "wx/osx/private.h"

@class wxTextEntryFormatter;

class wxNSTextBase : public wxWidgetCocoaImpl, public wxTextWidgetImpl
{
public :
    wxNSTextBase( wxTextCtrl *text, WXWidget w )
        : wxWidgetCocoaImpl(text, w),
          wxTextWidgetImpl(text)
    {
    }
    wxNSTextBase( wxWindow *wxPeer, wxTextEntry *entry, WXWidget w )
        : wxWidgetCocoaImpl(wxPeer, w),
          wxTextWidgetImpl(entry)
    {
    }
    virtual ~wxNSTextBase() { }

    virtual bool ShouldHandleKeyNavigation(const wxKeyEvent &event) const override;

    virtual void SetInitialLabel(const wxString& WXUNUSED(title), wxFontEncoding WXUNUSED(encoding)) override
    {
        // Don't do anything here, text controls don't have any label and
        // setting it would overwrite the string value set when creating it.
    }
};

// implementation exposed, so that search control can pull it

class wxNSTextFieldControl : public wxNSTextBase
{
public :
    // wxNSTextFieldControl must always be associated with a wxTextEntry. If
    // it's associated with a wxTextCtrl then we can get the associated entry
    // from it but otherwise the second ctor should be used to explicitly pass
    // us the entry.
    wxNSTextFieldControl( wxTextCtrl *text, WXWidget w );
    wxNSTextFieldControl( wxWindow *wxPeer, wxTextEntry *entry, WXWidget w );
    virtual ~wxNSTextFieldControl();

    virtual bool CanClipMaxLength() const override { return true; }
    virtual void SetMaxLength(unsigned long len) override;

    virtual bool CanForceUpper() override { return true; }
    virtual void ForceUpper() override;

    virtual wxString GetStringValue() const override ;
    virtual void SetStringValue( const wxString &str) override ;
    virtual void Copy() override ;
    virtual void Cut() override ;
    virtual void Paste() override ;
    virtual bool CanPaste() const override ;
    virtual void SetEditable(bool editable) override ;
    virtual long GetLastPosition() const override;
    virtual void GetSelection( long* from, long* to) const override ;
    virtual void SetSelection( long from , long to ) override;
    virtual bool PositionToXY(long pos, long *x, long *y) const override;
    virtual long XYToPosition(long x, long y) const override;
    virtual void ShowPosition(long pos) override;
    virtual void WriteText(const wxString& str) override ;
    virtual bool HasOwnContextMenu() const override { return true; }
    virtual bool SetHint(const wxString& hint) override;
    virtual void SetJustification() override;

    virtual void controlAction(WXWidget slf, void* _cmd, void *sender) override;
    virtual bool becomeFirstResponder(WXWidget slf, void *_cmd) override;
    virtual bool resignFirstResponder(WXWidget slf, void *_cmd) override;

    virtual void EnableNewLineReplacement(bool enable) override;
    virtual bool GetNewLineReplacement() override;
    virtual void SetInternalSelection( long from , long to );
    virtual void UpdateInternalSelectionFromEditor( wxNSTextFieldEditor* editor);

    virtual wxSize GetBestSize() const override;

protected :
    NSTextField* m_textField;
    long m_selStart;
    long m_selEnd;

private:
    // Common part of both ctors.
    void Init(WXWidget w);

    // Get our formatter, creating it if necessary.
    wxTextEntryFormatter* GetFormatter();
};

class wxNSTextViewControl : public wxNSTextBase
{
public:
    wxNSTextViewControl( wxTextCtrl *wxPeer, WXWidget w, long style );
    virtual ~wxNSTextViewControl();

    virtual void insertText(NSString* text, WXWidget slf, void *_cmd) override;

    virtual wxString GetStringValue() const override ;
    virtual void SetStringValue( const wxString &str) override ;
    virtual void Copy() override ;
    virtual void Cut() override ;
    virtual void Paste() override ;
    virtual bool CanPaste() const override ;
    virtual void SetEditable(bool editable) override ;
    virtual long GetLastPosition() const override;
    virtual void GetSelection( long* from, long* to) const override ;
    virtual void SetSelection( long from , long to ) override;
    virtual bool PositionToXY(long pos, long *x, long *y) const override;
    virtual long XYToPosition(long x, long y) const override;
    virtual void ShowPosition(long pos) override;
    virtual void WriteText(const wxString& str) override ;
    virtual void SetFont(const wxFont & font) override;

    virtual bool GetStyle(long position, wxTextAttr& style) override;
    virtual void SetStyle(long start, long end, const wxTextAttr& style) override;

    virtual bool CanFocus() const override;

    virtual bool HasOwnContextMenu() const override { return true; }

#if wxUSE_SPELLCHECK
    virtual void CheckSpelling(const wxTextProofOptions& options) override;
    virtual wxTextProofOptions GetCheckingOptions() const override;
#endif // wxUSE_SPELLCHECK
    virtual void EnableAutomaticQuoteSubstitution(bool enable) override;
    virtual void EnableAutomaticDashSubstitution(bool enable) override;
    virtual void EnableNewLineReplacement(bool enable) override;
    virtual bool GetNewLineReplacement() override;

    virtual wxSize GetBestSize() const override;
    virtual void SetJustification() override;

    virtual void controlTextDidChange() override;

    virtual bool CanUndo() const override;
    virtual void Undo() override;
    virtual bool CanRedo() const override;
    virtual void Redo() override;
    virtual void EmptyUndoBuffer() override;

protected:
    void DoUpdateTextStyle();

    NSScrollView* m_scrollView;
    NSTextView* m_textView;
    bool m_useCharWrapping;
    NSUndoManager* m_undoManager;
};

class wxNSComboBoxControl : public wxNSTextFieldControl, public wxComboWidgetImpl
{
public :
    wxNSComboBoxControl( wxComboBox *wxPeer, WXWidget w );
    virtual ~wxNSComboBoxControl();

    virtual int GetSelectedItem() const;
    virtual void SetSelectedItem(int item);

    virtual int GetNumberOfItems() const;

    virtual void InsertItem(int pos, const wxString& item);
    virtual void RemoveItem(int pos);

    virtual void Clear();

    virtual wxString GetStringAtIndex(int pos) const;

    virtual int FindString(const wxString& text) const;
    virtual void Popup();
    virtual void Dismiss();

    virtual void SetEditable(bool editable);

    virtual void mouseEvent(WX_NSEvent event, WXWidget slf, void *_cmd);

private:
    NSComboBox* m_comboBox;
};

#endif // _WX_OSX_COCOA_PRIVATE_TEXTIMPL_H_
