/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      William Osborne
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
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

#if wxUSE_CHOICE

#ifndef WX_PRECOMP
    #include "wx/choice.h"
    #include "wx/utils.h"
    #include "wx/log.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
#endif

#include "wx/palmos/private.h"

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
    return false;
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
    return FALSE;
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
    return false;
}

bool wxChoice::MSWShouldPreProcessMessage(WXMSG *pMsg)
{
    return false;
}

WXDWORD wxChoice::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    return 0;
}

wxChoice::~wxChoice()
{
}

// ----------------------------------------------------------------------------
// adding/deleting items to/from the list
// ----------------------------------------------------------------------------

int wxChoice::DoAppend(const wxString& item)
{
    return 0;
}

int wxChoice::DoInsert(const wxString& item, int pos)
{
    return 0;
}

void wxChoice::Delete(int n)
{
}

void wxChoice::Clear()
{
}

void wxChoice::Free()
{
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxChoice::GetSelection() const
{
    return 0;
}

void wxChoice::SetSelection(int n)
{
}

// ----------------------------------------------------------------------------
// string list functions
// ----------------------------------------------------------------------------

int wxChoice::GetCount() const
{
    return 0;
}

int wxChoice::FindString(const wxString& s) const
{
    return 0;
}

void wxChoice::SetString(int n, const wxString& s)
{
}

wxString wxChoice::GetString(int n) const
{
    wxString str;

    return str;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxChoice::DoSetItemClientData( int n, void* clientData )
{
}

void* wxChoice::DoGetItemClientData( int n ) const
{
    return (void *)NULL;
}

void wxChoice::DoSetItemClientObject( int n, wxClientData* clientData )
{
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
}

void wxChoice::DoMoveWindow(int x, int y, int width, int height)
{
}

void wxChoice::DoGetSize(int *w, int *h) const
{
}

void wxChoice::DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags)
{
}

wxSize wxChoice::DoGetBestSize() const
{
    return wxSize(0,0);
}

WXLRESULT wxChoice::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    return 0;
}

bool wxChoice::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    return false;
}

WXHBRUSH wxChoice::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
     )
{
    return (WXHBRUSH) 0;
}

#endif // wxUSE_CHOICE
