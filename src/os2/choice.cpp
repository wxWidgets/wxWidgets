/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/choice.h"
    #include "wx/utils.h"
    #include "wx/log.h"
#endif

#include "wx/os2/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n, const wxString choices[],
                      long style,
#if wxUSE_VALIDATORS
#  if defined(__VISAGECPP__)
                      const wxValidator* validator,
#  else
                      const wxValidator& validator,
#  endif
#endif
                      const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return FALSE;
// TODO:
/*
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

    if ( !OS2CreateControl(wxT("COMBOBOX"), msStyle) )
        return FALSE;

    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }
*/
    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

// ----------------------------------------------------------------------------
// adding/deleting items to/from the list
// ----------------------------------------------------------------------------

int wxChoice::DoAppend(const wxString& item)
{
    // TODO:
    /*
    int n = (int)SendMessage(GetHwnd(), CB_ADDSTRING, 0, (LONG)item.c_str());
    if ( n == CB_ERR )
    {
        wxLogLastError("SendMessage(CB_ADDSTRING)");
    }
    */
    return 0; //n
}

void wxChoice::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), wxT("invalid item index in wxChoice::Delete") );

// TODO:    SendMessage(GetHwnd(), CB_DELETESTRING, n, 0);
}

void wxChoice::Clear()
{
    // TODO: SendMessage(GetHwnd(), CB_RESETCONTENT, 0, 0);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxChoice::GetSelection() const
{
    // TODO: return (int)SendMessage(GetHwnd(), CB_GETCURSEL, 0, 0);
    return 0;
}

void wxChoice::SetSelection(int n)
{
    // TODO: SendMessage(GetHwnd(), CB_SETCURSEL, n, 0);
}

// ----------------------------------------------------------------------------
// string list functions
// ----------------------------------------------------------------------------

int wxChoice::GetCount() const
{
    // TODO: return (int)SendMessage(GetHwnd(), CB_GETCOUNT, 0, 0);
    return 0;
}

int wxChoice::FindString(const wxString& s) const
{
   // TODO:
   /*
    int pos = (int)SendMessage(GetHwnd(), CB_FINDSTRINGEXACT,
                               (WPARAM)-1, (LPARAM)s.c_str());

    return pos == LB_ERR ? wxNOT_FOUND : pos;
   */
    return 0;
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
    size_t len = 0; // TODO: (size_t)::SendMessage(GetHwnd(), CB_GETLBTEXTLEN, n, 0);
    wxString str = "";
    // TODO:
    /*
    if (len) {
        if ( ::SendMessage(GetHwnd(), CB_GETLBTEXT, n,
                           (LPARAM)str.GetWriteBuf(len)) == CB_ERR ) {
            wxLogLastError("SendMessage(CB_GETLBTEXT)");
        }
        str.UngetWriteBuf();
    }
    */
    return str;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxChoice::DoSetItemClientData( int n, void* clientData )
{
   // TODO:
   /*
    if ( SendMessage(GetHwnd(), CB_SETITEMDATA, n, (LPARAM)clientData) == CB_ERR )
    {
        wxLogLastError(wxT("CB_SETITEMDATA"));
    }
   */
}

void* wxChoice::DoGetItemClientData( int n ) const
{
 // TODO:
 /*
    LPARAM rc = SendMessage(GetHwnd(), CB_GETITEMDATA, n, 0);
    if ( rc == CB_ERR )
    {
        wxLogLastError(wxT("CB_GETITEMDATA"));

        // unfortunately, there is no way to return an error code to the user
	rc = (LPARAM) NULL;
    }

    return (void *)rc;
 */
    return NULL;
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
// wxOS2 specific helpers
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

MRESULT wxChoice::OS2WindowProc(HWND hwnd, WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
   // TODO:
   /*
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
    */
    return wxWindow::OS2WindowProc(hwnd, nMsg, wParam, lParam);
}

bool wxChoice::OS2Command(WXUINT param, WXWORD WXUNUSED(id))
{
   // TODO:
   /*
    if ( param != CBN_SELCHANGE)
    {
        // "selection changed" is the only event we're after
        return FALSE;
    }
    */
    wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_windowId);
    event.SetInt(GetSelection());
    event.SetEventObject(this);
// TODO:    event.SetString(GetStringSelection());
    ProcessCommand(event);

    return TRUE;
}

