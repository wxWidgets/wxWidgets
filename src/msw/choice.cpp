/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to derive from wxChoiceBase
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "choice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/choice.h"
    #include "wx/utils.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n, const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return FALSE;

    long msStyle = WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
    if ( style & wxCB_SORT )
        msStyle |= CBS_SORT;

    // the experience shows that wxChoice vs. wxComboBox distinction confuses
    // quite a few people - try to help them
    wxASSERT_MSG( !(style & wxCB_DROPDOWN) &&
                  !(style & wxCB_READONLY) &&
                  !(style & wxCB_SIMPLE),
                  wxT("this style flag is ignored by wxChoice, you "
                     "probably want to use a wxComboBox") );

    if ( !MSWCreateControl(wxT("COMBOBOX"), msStyle) )
        return FALSE;

    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

wxChoice::~wxChoice()
{
    Free();
}

// ----------------------------------------------------------------------------
// adding/deleting items to/from the list
// ----------------------------------------------------------------------------

int wxChoice::DoAppend(const wxString& item)
{
    int n = (int)SendMessage(GetHwnd(), CB_ADDSTRING, 0, (LONG)item.c_str());
    if ( n == CB_ERR )
    {
        wxLogLastError("SendMessage(CB_ADDSTRING)");
    }

    return n;
}

void wxChoice::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), wxT("invalid item index in wxChoice::Delete") );

    if ( HasClientObjectData() )
    {
        delete GetClientObject(n);
    }

    SendMessage(GetHwnd(), CB_DELETESTRING, n, 0);
}

void wxChoice::Clear()
{
    Free();

    SendMessage(GetHwnd(), CB_RESETCONTENT, 0, 0);
}

void wxChoice::Free()
{
    if ( HasClientObjectData() )
    {
        size_t count = GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            delete GetClientObject(n);
        }
    }
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxChoice::GetSelection() const
{
    return (int)SendMessage(GetHwnd(), CB_GETCURSEL, 0, 0);
}

void wxChoice::SetSelection(int n)
{
    SendMessage(GetHwnd(), CB_SETCURSEL, n, 0);
}

// ----------------------------------------------------------------------------
// string list functions
// ----------------------------------------------------------------------------

int wxChoice::GetCount() const
{
    return (int)SendMessage(GetHwnd(), CB_GETCOUNT, 0, 0);
}

int wxChoice::FindString(const wxString& s) const
{
#if defined(__WATCOMC__) && defined(__WIN386__)
    // For some reason, Watcom in WIN386 mode crashes in the CB_FINDSTRINGEXACT message.
    // wxChoice::Do it the long way instead.
    int count = GetCount();
    for ( int i = 0; i < count; i++ )
    {
        // as CB_FINDSTRINGEXACT is case insensitive, be case insensitive too
        if ( GetString(i).IsSameAs(s, FALSE) )
            return i;
    }

    return wxNOT_FOUND;
#else // !Watcom
    int pos = (int)SendMessage(GetHwnd(), CB_FINDSTRINGEXACT,
                               (WPARAM)-1, (LPARAM)s.c_str());

    return pos == LB_ERR ? wxNOT_FOUND : pos;
#endif // Watcom/!Watcom
}

void wxChoice::SetString(int n, const wxString& s)
{
    wxFAIL_MSG(wxT("not implemented"));

#if 0 // should do this, but no Insert() so far
    Delete(n);
    Insert(n + 1, s);
#endif
}

wxString wxChoice::GetString(int n) const
{
    size_t len = (size_t)::SendMessage(GetHwnd(), CB_GETLBTEXTLEN, n, 0);
    wxString str;
    if (len) {
        if ( ::SendMessage(GetHwnd(), CB_GETLBTEXT, n,
                           (LPARAM)str.GetWriteBuf(len)) == CB_ERR ) {
            wxLogLastError("SendMessage(CB_GETLBTEXT)");
        }
        str.UngetWriteBuf();
    }

    return str;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxChoice::DoSetItemClientData( int n, void* clientData )
{
    if ( SendMessage(GetHwnd(), CB_SETITEMDATA, n, (LPARAM)clientData) == CB_ERR )
    {
        wxLogLastError(wxT("CB_SETITEMDATA"));
    }
}

void* wxChoice::DoGetItemClientData( int n ) const
{
    LPARAM rc = SendMessage(GetHwnd(), CB_GETITEMDATA, n, 0);
    if ( rc == CB_ERR )
    {
        wxLogLastError(wxT("CB_GETITEMDATA"));

        // unfortunately, there is no way to return an error code to the user
        rc = (LPARAM) NULL;
    }

    return (void *)rc;
}

void wxChoice::DoSetItemClientObject( int n, wxClientData* clientData )
{
    DoSetItemClientData(n, clientData);
}

wxClientData* wxChoice::DoGetItemClientObject( int n ) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

// ----------------------------------------------------------------------------
// wxMSW specific helpers
// ----------------------------------------------------------------------------

void wxChoice::DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags)
{
    // Ignore height parameter because height doesn't mean 'initially
    // displayed' height, it refers to the drop-down menu as well. The
    // wxWindows interpretation is different; also, getting the size returns
    // the _displayed_ size (NOT the drop down menu size) so
    // setting-getting-setting size would not work.
    wxControl::DoSetSize(x, y, width, -1, sizeFlags);
}

wxSize wxChoice::DoGetBestSize()
{
    // find the widest string
    int wLine;
    int wChoice = 0;
    int nItems = GetCount();
    for ( int i = 0; i < nItems; i++ )
    {
        wxString str(GetString(i));
        GetTextExtent(str, &wLine, NULL);
        if ( wLine > wChoice )
            wChoice = wLine;
    }

    // give it some reasonable default value if there are no strings in the
    // list
    if ( wChoice == 0 )
        wChoice = 100;

    // the combobox should be larger than the widest string
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    wChoice += 5*cx;

    // Choice drop-down list depends on number of items (limited to 10)
    size_t nStrings = nItems == 0 ? 10 : wxMin(10, nItems) + 1;
    int hChoice = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)*nStrings;

    return wxSize(wChoice, hChoice);
}

long wxChoice::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( nMsg == WM_LBUTTONUP )
    {
        int x = (int)LOWORD(lParam);
        int y = (int)HIWORD(lParam);

        // Ok, this is truly weird, but if a panel with a wxChoice loses the
        // focus, then you get a *fake* WM_LBUTTONUP message with x = 65535 and
        // y = 65535. Filter out this nonsense.
        //
        // VZ: I'd like to know how to reproduce this please...
        if ( x == 65535 && y == 65535 )
            return 0;
    }

    return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}

bool wxChoice::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    if ( param != CBN_SELCHANGE)
    {
        // "selection changed" is the only event we're after
        return FALSE;
    }

    int n = GetSelection();
    if (n > -1)
    {
        wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_windowId);
        event.SetInt(n);
        event.SetEventObject(this);
        event.SetString(GetStringSelection());
        if ( HasClientObjectData() )
            event.SetClientObject( GetClientObject(n) );
        else if ( HasClientUntypedData() )
            event.SetClientData( GetClientData(n) );
        ProcessCommand(event);
    }

    return TRUE;
}

