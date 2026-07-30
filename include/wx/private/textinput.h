/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/textinput.h
// Purpose:     Internal interface for controls handling native text input
// Author:      Ryan Lucia
// Created:     2026-07-29
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_TEXTINPUT_H_
#define _WX_PRIVATE_TEXTINPUT_H_

#include "wx/defs.h"

#if defined(__WXGTK__) || defined(__WXOSX_COCOA__)

#include "wx/gdicmn.h"
#include "wx/string.h"

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxWindowBase;

// Implemented by generic controls that handle native text input themselves.
//
// This interface is deliberately private: native ports use it to delegate
// input method protocol calls without adding virtual functions to wxWindow.
class wxTextInputClient
{
public:
    // Sentinel positions used when a native text input API doesn't provide a
    // document position or explicitly refers to a non-existent one.
    static constexpr long NoPosition = -1;
    static constexpr long InvalidPosition = -2;

    virtual bool IsTextInputEnabled() const = 0;
    virtual bool HasActiveComposition() const = 0;
    virtual void CancelComposition() = 0;

#ifdef __WXGTK__
    virtual bool UpdateComposition(const wxString& text, int cursor) = 0;
    virtual bool CommitComposition(const wxString& text) = 0;
    virtual wxRect GetIMEContextRect() = 0;
#endif // __WXGTK__

#ifdef __WXOSX_COCOA__
    virtual bool InsertText(const wxString& text,
                            long replacementStart,
                            long replacementLength) = 0;
    virtual bool SetMarkedText(const wxString& text,
                               long selectedStart,
                               long selectedLength,
                               long replacementStart,
                               long replacementLength) = 0;
    virtual void UnmarkText() = 0;
    virtual bool HasMarkedText() const = 0;
    virtual bool GetMarkedTextRange(long* start, long* length) const = 0;
    virtual bool GetSelectedTextRange(long* start, long* length) const = 0;
    virtual bool GetTextInRange(long start, long length, wxString* text,
                                long* actualStart,
                                long* actualLength) const = 0;
    virtual bool GetTextRect(long start, long length, wxRect* rect,
                             long* actualStart,
                             long* actualLength) = 0;
    virtual bool GetTextPosition(const wxPoint& point, long* position) = 0;
#endif // __WXOSX_COCOA__

protected:
    virtual ~wxTextInputClient() = default;
};

// Associate a private text input client with a window. Passing nullptr as the
// client removes an existing association.
WXDLLIMPEXP_CORE
void wxAssociateTextInputClient(wxWindowBase* window,
                                wxTextInputClient* client);

WXDLLIMPEXP_CORE
wxTextInputClient* wxFindTextInputClient(const wxWindowBase* window);

#ifdef __WXGTK__
// Apply a changed text input mode to an already-created GTK IM context.
WXDLLIMPEXP_CORE
void wxUpdateTextInputClient(wxWindow* window);
#endif // __WXGTK__

#endif // __WXGTK__ || __WXOSX_COCOA__

#endif // _WX_PRIVATE_TEXTINPUT_H_
