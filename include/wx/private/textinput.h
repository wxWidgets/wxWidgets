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

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxWindowBase;

// Ports implementing the native text input protocol via wxTextInputClient.
#if defined(__WXGTK__) || defined(__WXOSX_COCOA__)
    #define wxHAS_TEXT_INPUT_CLIENT
#endif

#ifdef wxHAS_TEXT_INPUT_CLIENT

#include "wx/gdicmn.h"
#include "wx/string.h"

// Implemented by generic controls that handle native text input themselves.
// The port forwards its input method protocol calls to the window's
// associated client, if any.
//
// Only this part of the interface is common to all ports; the port-specific
// calls are defined by the wxTextInputClient specializations below.
class wxTextInputClientBase
{
public:
    // Sentinel positions used when a native text input API doesn't provide a
    // document position or explicitly refers to a non-existent one.
    static constexpr long NoPosition = -1;
    static constexpr long InvalidPosition = -2;

    virtual bool IsTextInputEnabled() const = 0;
    virtual bool HasActiveComposition() const = 0;
    virtual void CancelComposition() = 0;

protected:
    virtual ~wxTextInputClientBase() = default;
};

#if defined(__WXGTK__)

class wxTextInputClient : public wxTextInputClientBase
{
public:
    virtual bool UpdateComposition(const wxString& text, int cursor) = 0;
    virtual bool CommitComposition(const wxString& text) = 0;
    virtual wxRect GetIMEContextRect() = 0;

protected:
    ~wxTextInputClient() = default;
};

#elif defined(__WXOSX_COCOA__)

class wxTextInputClient : public wxTextInputClientBase
{
public:
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

protected:
    ~wxTextInputClient() = default;
};

#endif // port-specific wxTextInputClient definitions

// Associate a private text input client with a window. Passing nullptr as the
// client removes an existing association.
WXDLLIMPEXP_CORE
void wxAssociateTextInputClient(wxWindowBase* window,
                                wxTextInputClient* client);

WXDLLIMPEXP_CORE
wxTextInputClient* wxFindTextInputClient(const wxWindowBase* window);

// Make the native input method discard any text it's still composing for
// this window, without inserting it. Clients call this when the contents the
// composition belonged to are replaced entirely.
WXDLLIMPEXP_CORE
void wxResetTextInput(wxWindow* window);

#endif // wxHAS_TEXT_INPUT_CLIENT

#ifdef __WXGTK__

// Apply a changed text input mode to an already-created GTK IM context.
WXDLLIMPEXP_CORE
void wxUpdateTextInputClient(wxWindow* window);

#else // !__WXGTK__

// Trivial stub allowing the ports without a native context to update to call
// this function unconditionally.
inline void wxUpdateTextInputClient(wxWindow* WXUNUSED(window)) { }

#endif // __WXGTK__/!__WXGTK__

#endif // _WX_PRIVATE_TEXTINPUT_H_
