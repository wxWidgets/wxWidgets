/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to derive from wxChoiceBase
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "choice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CHOICE && !(defined(__SMARTPHONE__) && defined(__WXWINCE__))

#ifndef WX_PRECOMP
    #include "wx/choice.h"
    #include "wx/utils.h"
    #include "wx/log.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
#endif

#include "wx/msw/private.h"

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxChoiceStyle )

wxBEGIN_FLAGS( wxChoiceStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

wxEND_FLAGS( wxChoiceStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxChoice, wxControl,"wx/choice.h")

wxBEGIN_PROPERTIES_TABLE(wxChoice)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_CHOICE_SELECTED , wxCommandEvent )

    wxPROPERTY( Font , wxFont , SetFont , GetFont  , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_COLLECTION( Choices , wxArrayString , wxString , AppendString , GetStrings , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Selection ,int, SetSelection, GetSelection, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxChoiceStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxChoice)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_4( wxChoice , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size )
#else
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif
/*
    TODO PROPERTIES
        selection (long)
        content (list)
            item
*/

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
    // Experience shows that wxChoice vs. wxComboBox distinction confuses
    // quite a few people - try to help them
    wxASSERT_MSG( !(style & wxCB_DROPDOWN) &&
                  !(style & wxCB_READONLY) &&
                  !(style & wxCB_SIMPLE),
                  _T("this style flag is ignored by wxChoice, you ")
                  _T("probably want to use a wxComboBox") );

    return CreateAndInit(parent, id, pos, size, n, choices, style,
                         validator, name);
}

bool wxChoice::CreateAndInit(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             int n, const wxString choices[],
                             long style,
                             const wxValidator& validator,
                             const wxString& name)
{
    // initialize wxControl
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // now create the real HWND
    if ( !MSWCreateControl(wxT("COMBOBOX"), _T(""), pos, size) )
        return false;


    // choice/combobox normally has "white" (depends on colour scheme, of
    // course) background rather than inheriting the parent's background
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    // initialize the controls contents
    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }

    // and now we may finally size the control properly (if needed)
    SetBestSize(size);

    return true;
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

bool wxChoice::MSWShouldPreProcessMessage(WXMSG *pMsg)
{
    MSG *msg = (MSG *) pMsg;

    // if the dropdown list is visible, don't preprocess certain keys
    if ( msg->message == WM_KEYDOWN
        && (msg->wParam == VK_ESCAPE || msg->wParam == VK_RETURN) )
    {
        if (::SendMessage(GetHwndOf(this), CB_GETDROPPEDSTATE, 0, 0))
        {
            return false;
        }
    }

    return wxControl::MSWShouldPreProcessMessage(pMsg);
}

WXDWORD wxChoice::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    // we never have an external border
    WXDWORD msStyle = wxControl::MSWGetStyle
                      (
                        (style & ~wxBORDER_MASK) | wxBORDER_NONE, exstyle
                      );

    // WS_CLIPSIBLINGS is useful with wxChoice and doesn't seem to result in
    // any problems
    msStyle |= WS_CLIPSIBLINGS;

    // wxChoice-specific styles
    msStyle |= CBS_DROPDOWNLIST | WS_HSCROLL | WS_VSCROLL;
    if ( style & wxCB_SORT )
        msStyle |= CBS_SORT;

    return msStyle;
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
    int n = (int)SendMessage(GetHwnd(), CB_ADDSTRING, 0, (LPARAM)item.c_str());
    if ( n == CB_ERR )
    {
        wxLogLastError(wxT("SendMessage(CB_ADDSTRING)"));
    }
    else // ok
    {
        // we need to refresh our size in order to have enough space for the
        // newly added items
        if ( !IsFrozen() )
            UpdateVisibleHeight();
    }

    return n;
}

int wxChoice::DoInsert(const wxString& item, int pos)
{
    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG((pos>=0) && (pos<=GetCount()), -1, wxT("invalid index"));

    int n = (int)SendMessage(GetHwnd(), CB_INSERTSTRING, pos, (LPARAM)item.c_str());
    if ( n == CB_ERR )
    {
        wxLogLastError(wxT("SendMessage(CB_INSERTSTRING)"));
    }
    else // ok
    {
        if ( !IsFrozen() )
            UpdateVisibleHeight();
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

    if ( !IsFrozen() )
        UpdateVisibleHeight();
}

void wxChoice::Clear()
{
    Free();

    SendMessage(GetHwnd(), CB_RESETCONTENT, 0, 0);

    if ( !IsFrozen() )
        UpdateVisibleHeight();
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
        if ( GetString(i).IsSameAs(s, false) )
            return i;
    }

    return wxNOT_FOUND;
#else // !Watcom
   //TODO:  Evidently some MSW versions (all?) don't like empty strings
   //passed to SendMessage, so we have to do it ourselves in that case
   if ( s.size() == 0 )
   {
     int count = GetCount();
     for ( int i = 0; i < count; i++ )
     {
       if ( GetString(i).size() == 0 )
           return i;
     }

     return wxNOT_FOUND;
   }
   else
   {
     int pos = (int)SendMessage(GetHwnd(), CB_FINDSTRINGEXACT,
                                (WPARAM)-1, (LPARAM)s.c_str());
 
     return pos == LB_ERR ? wxNOT_FOUND : pos;
   }
#endif // Watcom/!Watcom
}

void wxChoice::SetString(int n, const wxString& s)
{
    wxCHECK_RET( n >= 0 && n < GetCount(),
                 wxT("invalid item index in wxChoice::SetString") );

    // we have to delete and add back the string as there is no way to change a
    // string in place

    // we need to preserve the client data
    void *data;
    if ( m_clientDataItemsType != wxClientData_None )
    {
        data = DoGetItemClientData(n);
    }
    else // no client data
    {
        data = NULL;
    }

    ::SendMessage(GetHwnd(), CB_DELETESTRING, n, 0);
    ::SendMessage(GetHwnd(), CB_INSERTSTRING, n, (LPARAM)s.c_str() );

    if ( data )
    {
        DoSetItemClientData(n, data);
    }
    //else: it's already NULL by default
}

wxString wxChoice::GetString(int n) const
{
    int len = (int)::SendMessage(GetHwnd(), CB_GETLBTEXTLEN, n, 0);

    wxString str;
    if ( len != CB_ERR && len > 0 )
    {
        if ( ::SendMessage
               (
                GetHwnd(),
                CB_GETLBTEXT,
                n,
                (LPARAM)(wxChar *)wxStringBuffer(str, len)
               ) == CB_ERR )
        {
            wxLogLastError(wxT("SendMessage(CB_GETLBTEXT)"));
        }
    }

    return str;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxChoice::DoSetItemClientData( int n, void* clientData )
{
    if ( ::SendMessage(GetHwnd(), CB_SETITEMDATA,
                       n, (LPARAM)clientData) == CB_ERR )
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

void wxChoice::UpdateVisibleHeight()
{
    // be careful to not change the width here
    DoSetSize(wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, GetSize().y, wxSIZE_USE_EXISTING);
}

void wxChoice::DoMoveWindow(int x, int y, int width, int height)
{
    // here is why this is necessary: if the width is negative, the combobox
    // window proc makes the window of the size width*height instead of
    // interpreting height in the usual manner (meaning the height of the drop
    // down list - usually the height specified in the call to MoveWindow()
    // will not change the height of combo box per se)
    //
    // this behaviour is not documented anywhere, but this is just how it is
    // here (NT 4.4) and, anyhow, the check shouldn't hurt - however without
    // the check, constraints/sizers using combos may break the height
    // constraint will have not at all the same value as expected
    if ( width < 0 )
        return;

    wxControl::DoMoveWindow(x, y, width, height);
}

void wxChoice::DoGetSize(int *w, int *h) const
{
    // this is weird: sometimes, the height returned by Windows is clearly the
    // total height of the control including the drop down list -- but only
    // sometimes, and normally it isn't... I have no idea about what to do with
    // this
    wxControl::DoGetSize(w, h);
}

void wxChoice::DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags)
{
    int heightOrig = height;

    // the height which we must pass to Windows should be the total height of
    // the control including the drop down list while the height given to us
    // is, of course, just the height of the permanently visible part of it
    if ( height != wxDefaultCoord )
    {
        // don't make the drop down list too tall, arbitrarily limit it to 40
        // items max and also don't leave it empty
        size_t nItems = GetCount();
        if ( !nItems )
            nItems = 9;
        else if ( nItems > 24 )
            nItems = 24;

        // add space for the drop down list
        const int hItem = SendMessage(GetHwnd(), CB_GETITEMHEIGHT, 0, 0);
        height += hItem*(nItems + 1);
    }

    wxControl::DoSetSize(x, y, width, height, sizeFlags);

    // I'm commenting this out since the code appears to make choices
    // and comboxes too high when they have associated sizers. I'm sure this
    // is not the end of the story, which is why I'm leaving it #if'ed out for
    // now. JACS.
#if 0
    // if the height specified for the visible part of the control is
    // different from the current one, we need to change it separately
    // as it is not affected by normal WM_SETSIZE
    if ( height != wxDefaultCoord )
    {
        const int delta = heightOrig - GetSize().y;
        if ( delta )
        {
            int h = ::SendMessage(GetHwnd(), CB_GETITEMHEIGHT, (WPARAM)-1, 0);
            SendMessage(GetHwnd(), CB_SETITEMHEIGHT, (WPARAM)-1, h + delta);
        }
    }
#else
    wxUnusedVar(heightOrig);
#endif
}

wxSize wxChoice::DoGetBestSize() const
{
    // find the widest string
    int wChoice = 0;
    const size_t nItems = GetCount();
    for ( size_t i = 0; i < nItems; i++ )
    {
        int wLine;
        GetTextExtent(GetString(i), &wLine, NULL);
        if ( wLine > wChoice )
            wChoice = wLine;
    }

    // give it some reasonable default value if there are no strings in the
    // list
    if ( wChoice == 0 )
        wChoice = 100;

    // the combobox should be slightly larger than the widest string
    wChoice += 5*GetCharWidth();

    return wxSize(wChoice, EDIT_HEIGHT_FROM_CHAR_HEIGHT(GetCharHeight()));
}

WXLRESULT wxChoice::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch ( nMsg )
    {
        case WM_LBUTTONUP:
            {
                int x = (int)LOWORD(lParam);
                int y = (int)HIWORD(lParam);

                // Ok, this is truly weird, but if a panel with a wxChoice
                // loses the focus, then you get a *fake* WM_LBUTTONUP message
                // with x = 65535 and y = 65535. Filter out this nonsense.
                //
                // VZ: I'd like to know how to reproduce this please...
                if ( x == 65535 && y == 65535 )
                    return 0;
            }
            break;

            // we have to handle both: one for the normal case and the other
            // for readonly
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSTATIC:
            {
                WXHDC hdc;
                WXHWND hwnd;
                UnpackCtlColor(wParam, lParam, &hdc, &hwnd);

                return (WXLRESULT)OnCtlColor(hdc, hwnd, 0,
                                             nMsg, wParam, lParam);
            }
    }

    return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}

bool wxChoice::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    if ( param != CBN_SELCHANGE)
    {
        // "selection changed" is the only event we're after
        return false;
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

    return true;
}

WXHBRUSH wxChoice::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
     )
{
    HDC hdc = (HDC)pDC;
    wxColour colBack = GetBackgroundColour();

    if (!IsEnabled())
        colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    ::SetBkColor(hdc, wxColourToRGB(colBack));
    ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

    return (WXHBRUSH)brush->GetResourceHandle();
}

#endif // wxUSE_CHOICE && !(__SMARTPHONE__ && __WXWINCE__)
