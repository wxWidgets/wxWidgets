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

    virtual bool ShouldHandleKeyNavigation(const wxKeyEvent &event) const wxOVERRIDE;

    virtual void SetInitialLabel(const wxString& WXUNUSED(title), wxFontEncoding WXUNUSED(encoding)) wxOVERRIDE
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

    virtual bool CanClipMaxLength() const wxOVERRIDE { return true; }
    virtual void SetMaxLength(unsigned long len) wxOVERRIDE;

    virtual bool CanForceUpper() wxOVERRIDE { return true; }
    virtual void ForceUpper() wxOVERRIDE;

    virtual wxString GetStringValue() const wxOVERRIDE ;
    virtual void SetStringValue( const wxString &str) wxOVERRIDE ;
    virtual void Copy() wxOVERRIDE ;
    virtual void Cut() wxOVERRIDE ;
    virtual void Paste() wxOVERRIDE ;
    virtual bool CanPaste() const wxOVERRIDE ;
    virtual void SetEditable(bool editable) wxOVERRIDE ;
    virtual long GetLastPosition() const wxOVERRIDE;
    virtual void GetSelection( long* from, long* to) const wxOVERRIDE ;
    virtual void SetSelection( long from , long to ) wxOVERRIDE;
    virtual bool PositionToXY(long pos, long *x, long *y) const wxOVERRIDE;
    virtual long XYToPosition(long x, long y) const wxOVERRIDE;
    virtual void ShowPosition(long pos) wxOVERRIDE;
    virtual void WriteText(const wxString& str) wxOVERRIDE ;
    virtual bool HasOwnContextMenu() const wxOVERRIDE { return true; }
    virtual bool SetHint(const wxString& hint) wxOVERRIDE;
    virtual void SetJustification() wxOVERRIDE;

    virtual void controlAction(WXWidget slf, void* _cmd, void *sender) wxOVERRIDE;
    virtual bool becomeFirstResponder(WXWidget slf, void *_cmd) wxOVERRIDE;
    virtual bool resignFirstResponder(WXWidget slf, void *_cmd) wxOVERRIDE;

    virtual void SetInternalSelection( long from , long to );
    virtual void UpdateInternalSelectionFromEditor( wxNSTextFieldEditor* editor);
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

    virtual void insertText(NSString* text, WXWidget slf, void *_cmd) wxOVERRIDE;

    virtual wxString GetStringValue() const wxOVERRIDE ;
    virtual void SetStringValue( const wxString &str) wxOVERRIDE ;
    virtual void Copy() wxOVERRIDE ;
    virtual void Cut() wxOVERRIDE ;
    virtual void Paste() wxOVERRIDE ;
    virtual bool CanPaste() const wxOVERRIDE ;
    virtual void SetEditable(bool editable) wxOVERRIDE ;
    virtual long GetLastPosition() const wxOVERRIDE;
    virtual void GetSelection( long* from, long* to) const wxOVERRIDE ;
    virtual void SetSelection( long from , long to ) wxOVERRIDE;
    virtual bool PositionToXY(long pos, long *x, long *y) const wxOVERRIDE;
    virtual long XYToPosition(long x, long y) const wxOVERRIDE;
    virtual void ShowPosition(long pos) wxOVERRIDE;
    virtual void WriteText(const wxString& str) wxOVERRIDE ;
    virtual void SetFont( const wxFont & font , const wxColour& foreground , long windowStyle, bool ignoreBlack = true ) wxOVERRIDE;

    virtual bool GetStyle(long position, wxTextAttr& style) wxOVERRIDE;
    virtual void SetStyle(long start, long end, const wxTextAttr& style) wxOVERRIDE;

    virtual bool CanFocus() const wxOVERRIDE;

    virtual bool HasOwnContextMenu() const wxOVERRIDE { return true; }

    virtual void CheckSpelling(bool check) wxOVERRIDE;
    virtual void EnableAutomaticQuoteSubstitution(bool enable) wxOVERRIDE;
    virtual void EnableAutomaticDashSubstitution(bool enable) wxOVERRIDE;

    virtual wxSize GetBestSize() const wxOVERRIDE;
    virtual void SetJustification() wxOVERRIDE;

    virtual void controlTextDidChange() wxOVERRIDE;

protected:
    void DoUpdateTextStyle();

    NSScrollView* m_scrollView;
    NSTextView* m_textView;
    bool m_useCharWrapping;
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
