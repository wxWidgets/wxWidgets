/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_STATTEXT

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/brush.h"
#endif

#include "wx/stattext.h"
#include "wx/msw/private.h"
#include <stdio.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    if ( !MSWCreateControl(wxT("STATIC"), label, pos, size) )
        return FALSE;

    return TRUE;
}

wxBorder wxStaticText::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

WXDWORD wxStaticText::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // translate the alignment flags to the Windows ones
    //
    // note that both wxALIGN_LEFT and SS_LEFT are equal to 0 so we shouldn't
    // test for them using & operator
    if ( style & wxALIGN_CENTRE )
        msStyle |= SS_CENTER;
    else if ( style & wxALIGN_RIGHT )
        msStyle |= SS_RIGHT;
    else
        msStyle |= SS_LEFT;

    return msStyle;
}

wxSize wxStaticText::DoGetBestSize() const
{
    wxString text(wxGetWindowText(GetHWND()));

    int widthTextMax = 0, widthLine,
        heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    bool lastWasAmpersand = FALSE;

    wxString curLine;
    for ( const wxChar *pc = text; ; pc++ )
    {
        if ( *pc == wxT('\n') || *pc == wxT('\0') )
        {
            if ( !curLine )
            {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;
                if ( !heightLineDefault )
                    GetTextExtent(_T("W"), NULL, &heightLineDefault);

                heightTextTotal += heightLineDefault;
            }
            else
            {
                GetTextExtent(curLine, &widthLine, &heightLine);
                if ( widthLine > widthTextMax )
                    widthTextMax = widthLine;
                heightTextTotal += heightLine;
            }

            if ( *pc == wxT('\n') )
            {
               curLine.Empty();
            }
            else
            {
               // the end of string
               break;
            }
        }
        else
        {
            // we shouldn't take into account the '&' which just introduces the
            // mnemonic characters and so are not shown on the screen -- except
            // when it is preceded by another '&' in which case it stands for a
            // literal ampersand
            if ( *pc == _T('&') )
            {
                if ( !lastWasAmpersand )
                {
                    lastWasAmpersand = TRUE;

                    // skip the statement adding pc to curLine below
                    continue;
                }

                // it is a literal ampersand
                lastWasAmpersand = FALSE;
            }

            curLine += *pc;
        }
    }

    return wxSize(widthTextMax, heightTextTotal);
}

void wxStaticText::DoSetSize(int x, int y, int w, int h, int sizeFlags)
{
    // we need to refresh the window after changing its size as the standard
    // control doesn't always update itself properly
    wxStaticTextBase::DoSetSize(x, y, w, h, sizeFlags);

    Refresh();
}

void wxStaticText::SetLabel(const wxString& label)
{
    wxStaticTextBase::SetLabel(label);

    // adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }
}


bool wxStaticText::SetFont(const wxFont& font)
{
    bool ret = wxControl::SetFont(font);

    // adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }

    return ret;
}

#endif // wxUSE_STATTEXT
